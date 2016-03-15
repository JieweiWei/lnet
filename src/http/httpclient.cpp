/*
 * ==================================================================================
 *
 *          Filename: httpclient.cpp
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

#include "httpclient.h"

#include "httpconnector.h"
#include "address.h"
#include "httprequest.h"
#include "logger.h"
#include "sockutil.h"
#include "connection.h"

using namespace std;

LNET_NAMESPACE_BEGIN

HttpClient::HttpClient(IOLoop *loop, int maxClients)
    : m_loop(loop)
    , m_maxClients(maxClients) {
}

HttpClient::~HttpClient() {
    for (auto it = m_connectors.begin(); it != m_connectors.end(); ++it) {
        shared_ptr<HttpConnector> con = it->second.lock();
        if (con) {
            con->shutdown();
        }
    }
    m_connectors.clear();
}

void HttpClient::request(
    const string &url,
    const ResponseCallback &callback,
    enum http_method method,
    const Headers &headers,
    const string &body) {
    HttpRequest request;
    request.url = url;
    request.headers = headers;
    request.body = body;
    request.method = method;
    this->request(request, callback);
}

void HttpClient::request(HttpRequest &request, const ResponseCallback &callback) {
    request.parseUrl();
    Address addr(request.host, request.port);
    shared_ptr<HttpConnector> con = popConnect(addr.ip());
    if (con) {
        con->setCallback(bind(&HttpClient::onResponse, this, _1, _2, _3, callback));
        con->send(request.dump());
    } else {
        con = make_shared<HttpConnector>();
        con->connect(m_loop, addr, bind(&HttpClient::onConnect, this, _1, _2, request.dump(), callback), m_device);
    }
}

void HttpClient::onResponse(
    const shared_ptr<HttpConnector> &con,
    const HttpResponse &resp,
    RESPONSE_EVENT event,
    const ResponseCallback &callback) {
    if (event == RESPONSE_COMPLETE) {
        pushConnect(con);
    }
    callback(resp);
}

void HttpClient::onConnect(
    const shared_ptr<HttpConnector> &con,
    bool connected,
    const string &requestData,
    const ResponseCallback &callback) {
    if (!connected) {
        LOG_ERROR("httpclient connect error");
        return;
    }
    // ??? move
    con->setCallback(bind(&HttpClient::onResponse, this, _1, _2, _3, callback));
    con->send(requestData);
}

void HttpClient::pushConnect(const shared_ptr<HttpConnector> &con) {
    con->clearCallback();
    shared_ptr<Connection> c = con->lockConnection();
    if (!c) {
        return;
    }
    if (m_connectors.size() >= (size_t)m_maxClients) {
        con->shutdown();
        return;
    }
    Address addr(0);
    if (SockUtil::getRemoteAddr(c->getSockFd(), addr) != 0) {
        con->shutdown();
        return;
    }
    m_connectors.insert(make_pair(addr.ip(), con));
}

shared_ptr<HttpConnector> HttpClient::popConnect(uint32_t ip) {
    while (true) {
        auto it = m_connectors.find(ip);
        if (it == m_connectors.end()) {
            return shared_ptr<HttpConnector>();
        } else {
            weak_ptr<HttpConnector> con = it->second;
            m_connectors.erase(it);
            shared_ptr<HttpConnector> c = con.lock();
            if (c && c->lockConnection()) {
                return c;
            }
        }
    }
}

LNET_NAMESPACE_END
