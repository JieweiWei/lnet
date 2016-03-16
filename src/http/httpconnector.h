/*
 * ==================================================================================
 *
 *          Filename: httpconnector.h
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

#ifndef __LNET_HTTPCONNECTOR_H__
#define __LNET_HTTPCONNECTOR_H__

#include "lnethttp.h"

#include "httpresponse.h"
#include "connector.h"
#include "httpparser.h"

LNET_NAMESPACE_BEGIN

class HttpConnector : public HttpParser, public Connector<HttpConnector> {
public:
    friend class HttpClient;
    friend class Connector<HttpConnector>;

    using ResponseCallback = std::function<void (const std::shared_ptr<HttpConnector>&, const HttpResponse&, RESPONSE_EVENT)>;

    HttpConnector();
    ~HttpConnector();

    void setCallback(const ResponseCallback &callback) { m_callback = callback; }
    void clearCallback();

    static void setMaxHeadersSize(size_t size) { ms_maxHeaderSize = size; }
    static void setMaxBodySize(size_t size) { ms_maxBodySize = size; }

private:
    void headRead(const char *buf, size_t count);
    int onMessageBegin();
    int onHeader(const std::string &field, const std::string &value);
    int onHeadersComplete();
    int onBody(const char *at, size_t length);
    int onMessageComplete();
    int onError(const HttpError &error);

private:
    HttpResponse m_response;
    ResponseCallback m_callback;
    static size_t ms_maxHeaderSize;
    static size_t ms_maxBodySize;

}; /* end of class HttpConnector */

LNET_NAMESPACE_END

#endif /* end of define __LNET_HTTPCONNECTOR_H__ */
