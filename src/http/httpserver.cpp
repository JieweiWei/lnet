/*
 * ==================================================================================
 *
 *          Filename: httpserver.cpp
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

#include "httpserver.h"

#include "tcpserver.h"
#include "connection.h"
#include "logger.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "httpconnection.h"

using namespace std;

LNET_NAMESPACE_BEGIN

HttpServer::HttpServer(TcpServer *server) : m_tcpserver(server) {
}

HttpServer::~HttpServer() {
}

int HttpServer::listen(const Address &addr) {
    return m_tcpserver->listen(addr, bind(&HttpServer::onConnectEvent, this, _1, _2, _3));
}

void HttpServer::onConnectEvent(const shared_ptr<Connection> &con, CONNECT_EVENT event, const void *context) {
    switch (event) {
        case ESTABLISHED: {
            shared_ptr<HttpConnection> httpcon = make_shared<HttpConnection>(
                con,
                bind(&HttpServer::onRequest, this, _1, _2, _3)
            );
            con->setEventCallback(
                bind(&HttpConnection::onConnectEvent, httpcon, _1, _2, _3)
            );
            break;
        }
        default: {
            LOG_INFO("error when enter this");
            break;
        }
    }
}

void HttpServer::onRequest(const shared_ptr<HttpConnection> &con, const HttpRequest &request,
    REQUEST_EVENT event, const void *context) {
    switch (event) {
        case REQUEST_UPGRADE:
            // TODO web socket
            LOG_INFO("not support web socket");
            break;
        case REQUEST_ERROR:
            onError(con, *(HttpError*)context);
            break;
        case REQUEST_COMPLETE: {
            auto it = m_httpCallbacks.find(request.path);
            if (it == m_httpCallbacks.end()) {
                HttpResponse response;
                response.code = 404;
                con->send(response);
            } else {
                if (authRequest(con, request)) {
                    (it->second)(con, request);
                }
            }
            break;
        }
        default:
            LOG_ERROR("invalid request event %d", event);
            break;
    }
}

void HttpServer::onError(const shared_ptr<HttpConnection> &con, const HttpError &error) {
    con->send(error.code, error.message);
    con->shutdown(1000);
}

bool HttpServer::authRequest(const shared_ptr<HttpConnection> &con, const HttpRequest &request) {
    auto it = m_authCallbacks.find(request.path);
    if (it == m_authCallbacks.end()) {
        return true;
    }
    HttpError error = (it->second)(request);
    if (error.code != 200) {
        onError(con, error);
        return false;
    }
    return true;
}

LNET_NAMESPACE_END
