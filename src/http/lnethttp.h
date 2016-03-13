/*
 * ==================================================================================
 *
 *          Filename: lnethttp.h
 *
 *       Description:
 *
 *            Verson: 1.0
 *           Created: 2016-03-11
 *          Compiler: g++
 *
 *            Author: Jiewei Wei <weijieweijerry@163.com>
 *           Company: Sun Yat-sen University
 *
 * ==================================================================================
 */

#ifndef __LNET_HTTP_H__
#define __LNET_HTTP_H__

#include "lnet.h"

#include <map>
#include <string.h>

LNET_NAMESPACE_BEGIN

class HttpConnection;
class HttpRequest;

struct HttpError {
    HttpError(int pcode = 200, const std::string &msg = "")
        : code(pcode)
        , message(msg) {
    }
    int code;
    std::string message;
};

struct CaseKeyCmp {
    bool operator() (const std::string &str1, const std::string &str2) const {
        return strcasecmp(str1.c_str(), str2.c_str()) < 0;
    }
};

enum REQUEST_EVENT {
    REQUEST_UPGRADE,
    REQUEST_COMPLETE,
    REQUEST_ERROR
};

using Headers      = std::multimap<std::string, std::string, CaseKeyCmp>;
using Params       = std::multimap<std::string, std::string>;
using HttpCallback = std::function<void (const std::shared_ptr<HttpConnection>&, const HttpRequest&)>;
using AuthCallback = std::function<HttpError (const HttpRequest&)>;

LNET_NAMESPACE_END

#endif /* end of define __LNET_HTTP_H__ */
