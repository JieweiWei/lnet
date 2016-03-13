/*
 * ==================================================================================
 *
 *          Filename: httprequest.h
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

#ifndef __LNET_HTTPREQUEST_H__
#define __LNET_HTTPREQUEST_H__

#include "lnethttp.h"

#include <http_parser.h>

#include <string>

LNET_NAMESPACE_BEGIN

class HttpRequest {
public:
    HttpRequest();
    ~HttpRequest();

    void clear();
    void parseUrl();
    std::string dump();
    void parseQuery();

public:
    std::string url;
    std::string body;
    std::string schema;
    std::string host;
    std::string path;
    std::string query;
    Headers headers;
    Params params;
    unsigned short majorVersion;
    unsigned short minorVersion;
    http_method method;
    uint16_t port;

}; /* end of class HttpRequest */

LNET_NAMESPACE_END

#endif /* end of define __LNET_HTTPREQUEST_H__ */
