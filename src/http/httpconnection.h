/*
 * ==================================================================================
 *
 *          Filename: httpconnection.h
 *
 *       Description:
 *
 *            Verson: 1.0
 *           Created: 2016-03-13
 *          Compiler: g++
 *
 *            Author: Jiewei Wei <weijieweijerry@163.com>
 *           Company: Sun Yat-sen University
 *
 * ==================================================================================
 */

#ifndef __LNET_HTTPCONNECTION_H__
#define __LNET_HTTPCONNECTION_H__


#include "lnethttp.h"

#include "httpparser.h"

LNET_NAMESPACE_BEGIN

class HttpConnection : public HttpParser {
public:
    friend class HttpServer;
    using RequestCallback = std::function<void (std::shared_ptr<HttpConnection>&, const HttpRequest&, REQUEST_EVENT, const void*)>;

}; /* end of HttpConnection */

LNET_NAMESPACE_END

#endif /* end of define __LNET_HTTPCONNECTION_H__ */
