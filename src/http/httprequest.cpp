/*
 * ==================================================================================
 *
 *          Filename: httprequest.cpp
 *
 *       Description:
 *
 *            Verson: 1.0
 *           Created: 2016-03-12
 *          Compiler: g++
 *
 *            Author: Jiewei Wei <weijieweijerry@163.com>
 *           Company: Sun Yat-sen University
 *
 * ==================================================================================
 */

#include "httprequest.h"

#include "logger.h"
#include "stringutil.h"
#include "httputil.h"

extern "C" {
    #include "http_parser.h"
}

using namespace std;

LNET_NAMESPACE_BEGIN

HttpRequest::HttpRequest()
    : majorVersion(1)
    , minorVersion(1)
    , method(HTTP_GET) {
}

HttpRequest::~HttpRequest() {
}

void HttpRequest::clear() {
    url.clear();
    schema.clear();
    host.clear();
    path.clear();
    query.clear();
    body.clear();
    headers.clear();
    params.clear();
    majorVersion = 1;
    minorVersion = 1;
    method = HTTP_GET;
    port = 80;
}

void HttpRequest::parseUrl() {
    if (!schema.empty()) {
        return;
    }
    struct http_parser_url t_url;
    if (http_parser_parse_url(url.c_str(), url.size(), 0, &t_url) != 0) {
        LOG_ERROR("parseurl error %s", url.c_str());
    }
    if (t_url.field_set & (1 << UF_SCHEMA)) {
        schema = url.substr(t_url.field_data[UF_SCHEMA].off, t_url.field_data[UF_SCHEMA].len);
    }
    if (t_url.field_set & (1 << UF_HOST)) {
        host = url.substr(t_url.field_data[UF_HOST].off, t_url.field_data[UF_HOST].len);
    }
    if (t_url.field_set & (1 << UF_PORT)) {
        port = t_url.port;
    } else {
        if (strcasecmp(schema.c_str(), "https") == 0 || strcasecmp(schema.c_str(), "wss") == 0) {
            port = 443;
        } else {
            port = 80;
        }
    }
    if (t_url.field_set & (1 << UF_PATH)) {
        path = url.substr(t_url.field_data[UF_PATH].off, t_url.field_data[UF_PATH].len);
    }
    if (t_url.field_set & (1 << UF_QUERY)) {
        query = url.substr(t_url.field_data[UF_QUERY].off, t_url.field_data[UF_QUERY].len);
        parseQuery();
    }
}

string HttpRequest::dump() {
    static const string HostKey = "Host";
    static const string ContentLengthKey = "Content-Length";

    parseUrl();
    char buf[1024] = {0};
    path = path.empty() ? "/" : path;
    int n = 0;
    if (query.empty()) {
        n = snprintf(buf, sizeof(buf), "%s %s HTTP/%d.%d\r\n",
            http_method_str(method), path.c_str(), majorVersion, minorVersion);
    }
    string ret(buf, n);
    headers.erase(HostKey);
    if (port == 80 || port == 443) {
        headers.insert(make_pair(HostKey, host));
    } else {
        n = snprintf(buf, sizeof(buf), "%s:%d", host.c_str(), port);
        headers.insert(make_pair(HostKey, string(buf, n)));
    }
    if (method == HTTP_POST || method == HTTP_PUT) {
        headers.erase(ContentLengthKey);
        n = snprintf(buf, sizeof(buf), "%d", int(body.size()));
        headers.insert(make_pair(ContentLengthKey, string(buf, n)));
    }
    for (auto it = headers.cbegin(); it != headers.cend(); ++ it) {
        n = snprintf(buf, sizeof(buf), "%s: %s\r\n", it->first.c_str(), it->second.c_str());
        ret.append(buf, n);
    }
    return ret.append("\r\n").append(body);
}

void HttpRequest::parseQuery() {
    if (query.empty() || !params.empty()) {
        return;
    }
    static const string sep1 = "&";
    static const string sep2 = "=";
    vector<string> args = StringUtil::split(query, sep1);
    pair<string, string> keyValue;
    for (auto arg : args) {
        vector<string> parts = StringUtil::split(arg, sep2);
        if (parts.size() == 2) {
            keyValue = make_pair(parts[0], parts[1]);
        } else if (parts.size() == 1) {
            keyValue = make_pair(parts[0], "");
        } else {
            continue;
        }
        params.insert(make_pair(HttpUtil::unescape(keyValue.first), HttpUtil::unescape(keyValue.second)));
    }
}

LNET_NAMESPACE_END
