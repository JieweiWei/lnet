/*
 * ==================================================================================
 *
 *          Filename: httpserver.h
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

#ifndef __LNET_HTTPSERVER_H__
#define __LNET_HTTPSERVER_H__

#include "lnethttp.h"

LNET_NAMESPACE_BEGIN

class TcpServer;
class Address;

class HttpServer {
public:
    HttpServer(TcpServer *server);
    ~HttpServer();
    DISALLOW_COPY_AND_ASSIGN(HttpServer);

    int listen(const Address &addr);

    void setHttpCallback(const std::string &path, const HttpCallback &callback) {
        m_httpCallbacks[path] = callback;
    }
    void setHttpCallback(const std::string &path, const HttpCallback &callback, const AuthCallback &auth) {
        m_httpCallbacks[path] = callback;
        m_authCallbacks[path] = auth;
    }
    //void setHttpCallback(const std::string &path, const HttpCallback &callback);
    //void setHttpCallback(const std::string &path, const HttpCallback &callback, const AuthCallback &auth);

private:
    void onConnectEvent(const std::shared_ptr<Connection>& con,
        CONNECT_EVENT event, const void *context);
    void onRequest(const std::shared_ptr<HttpConnection> &con,
        const HttpRequest &request, REQUEST_EVENT event, const void *context);
    void onError(const std::shared_ptr<HttpConnection> &con, const HttpError &error);
    bool authRequest(const std::shared_ptr<HttpConnection> &con, const HttpRequest &request);

private:
    TcpServer *m_tcpserver;
    std::map <std::string, HttpCallback> m_httpCallbacks;
    // TODO: web socket
    std::map<std::string, AuthCallback> m_authCallbacks;

}; /* end of class HttpServer */

LNET_NAMESPACE_END

#endif /* end of define __LNET_HTTPSERVER_H__ */
