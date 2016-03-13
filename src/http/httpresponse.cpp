/*
 * ==================================================================================
 *
 *          Filename: httpresponse.cpp
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

#include "httpresponse.h"

#include "httputil.h"

using namespace std;

LNET_NAMESPACE_BEGIN

HttpResponse::HttpResponse() : code(200) {
}

HttpResponse::HttpResponse(int pcode, const Headers &headers, const string &body)
    : code(pcode), headers(headers), body(body) {
}

HttpResponse::~HttpResponse() {
}

void HttpResponse::setContentType(const string &contentType) {
    static const string ContentTypeKey = "Content-Type";
    headers.insert(make_pair(ContentTypeKey, contentType));
}

void HttpResponse::setKeepAlive(bool on) {
    static const string ConnectionKey = "Connection";
    if (on) {
        static const string KeepAliveValue = "Keep-Alive";
        headers.insert(make_pair(ConnectionKey, KeepAliveValue));
    } else {
        static const string CloseValue = "close";
        headers.insert(make_pair(ConnectionKey, CloseValue));
    }
}

void HttpResponse::enableDate() {
    time_t now = time(NULL);
    struct tm t;
    gmtime_r(&now, &t);
    char buf[128] = {0};
    int n = strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &t);
    static const string DateKey = "Date";
    headers.insert(make_pair(DateKey, string(buf, n)));
}

string HttpResponse::dump() {
    char buf[1024] = {0};
    int n = snprintf(buf, sizeof(buf), "HTTP/1.1 %d %s\r\n", code, HttpUtil::codeMessage(code).c_str());
    string ret(buf, n);
    n = snprintf(buf, sizeof(buf), "%d", int(body.size()));
    static const string ContentLengthKey = "Content-Length";
    headers.insert(make_pair(ContentLengthKey, string(buf, n)));
    for (auto it = headers.cbegin(); it != headers.end(); ++it) {
        n = snprintf(buf, sizeof(buf), "%s: %s\r\n", it->first.c_str(), it->second.c_str());
    }
    return ret.append("\r\n").append(body);
}

LNET_NAMESPACE_END
