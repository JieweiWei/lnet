/*
 * ==================================================================================
 *
 *          Filename: httpresponse.h
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

#ifndef __LNET_HTTPRESPONSE_H__
#define __LNET_HTTPRESPONSE_H__

#include "lnethttp.h"

LNET_NAMESPACE_BEGIN

class HttpResponse {
public:
    HttpResponse();
    HttpResponse(int code, const Headers &headers = Headers(), const std::string &body = "");
    ~HttpResponse();

    void clear() {
        code = 200;
        body.clear();
        headers.clear();
    }

    void setContentType(const std::string &contentType);
    void setKeepAlive(bool on = true);
    void enableDate();
    std::string dump();

    int code;
    Headers headers;
    std::string body;

}; /* end of class HttpResponse */

LNET_NAMESPACE_END

#endif /* end of define __LNET_HTTPRESPONSE_H__ */
