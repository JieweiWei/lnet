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
#include "httprequest.h"

LNET_NAMESPACE_BEGIN

class HttpResponse;

class HttpConnection
    : public HttpParser
    , public std::enable_shared_from_this<HttpConnection> {
public:
    friend class HttpServer;

    using RequestCallback = std::function<void (const std::shared_ptr<HttpConnection>&, const HttpRequest&, REQUEST_EVENT, const void*)>;

    HttpConnection(const std::shared_ptr<Connection> &con, const RequestCallback &callback);
    ~HttpConnection();

    int getSockFd() const { return m_sockFd; }

    void send(HttpResponse &response);
    void send(int code, const std::string &body = "", const Headers &headers = Headers());
    void send(HttpResponse &response, const Callback &callback);
    void send(int code, const Callback &callback, const std::string &body = "", const Headers &headers = Headers());

    void shutdown(int after);;
    std::shared_ptr<Connection> lockConnection() const { return m_connection.lock(); }
    std::weak_ptr<Connection> getConnecion() const { return m_connection; }

    static void setMaxHeadersSize(size_t size) { ms_maxHeaderSize = size; }
    static void setMaxBodySize(size_t size) { ms_maxBodySize = size; }

private:
    int onMessageBegin();
    int onUrl(const char *at, size_t length);
    int onHeader(const std::string &field, const std::string &value);
    int onHeadersComplete();
    int onBody(const char *at, size_t length);
    int onMessageComplete();
    int onUpgrade(const char *at, size_t length);
    int onError(const HttpError &error);

    void onConnectEvent(const std::shared_ptr<Connection> &con, CONNECT_EVENT event, const void *context);

public:
    // ???
    std::weak_ptr<Connection> m_connection;
    int m_sockFd;
    HttpRequest m_request;
    RequestCallback m_callback;
    Callback m_sendCallback;
    static size_t ms_maxHeaderSize;
    static size_t ms_maxBodySize;

}; /* end of HttpConnection */

LNET_NAMESPACE_END

#endif /* end of define __LNET_HTTPCONNECTION_H__ */
