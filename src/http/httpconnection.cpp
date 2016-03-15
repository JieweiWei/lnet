/*
 * ==================================================================================
 *
 *          Filename: httpconnection.cpp
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

#include "httpconnection.h"

#include "connection.h"
#include "logger.h"
#include "httpresponse.h"

using namespace std;

LNET_NAMESPACE_BEGIN

size_t HttpConnection::ms_maxHeaderSize = 80 * 1024;
size_t HttpConnection::ms_maxBodySize   = 10 * 1024 * 1024;

static void dummyCallback() {
}

HttpConnection::HttpConnection(const shared_ptr<Connection> &con,
    const RequestCallback &callback)
    : HttpParser(HTTP_REQUEST)
    , m_connection(con)
    , m_callback(callback) {
    m_sockFd = con->getSockFd();
    m_sendCallback = bind(&dummyCallback);
}

HttpConnection::~HttpConnection() {
    LOG_INFO("httpconnection destroyed");
}

void HttpConnection::send(HttpResponse &response) {
    shared_ptr<Connection> con = lockConnection();
    if (con) {
        con->send(response.dump());
    }
}

void HttpConnection::send(int code, const string &body, const Headers &headers) {
    HttpResponse response(code, headers, body);
    send(response);
}

void HttpConnection::send(HttpResponse &response, const Callback &callback) {
    m_sendCallback = callback;
    send(response);
}

void HttpConnection::send(int code, const Callback &callback,
    const std::string &body, const Headers &headers) {
    HttpResponse response(code, headers, body);
    send(response, callback);
}

void HttpConnection::shutdown(int after) {
    shared_ptr<Connection> con = lockConnection();
    if (con) {
        con->shutdown(after);
    }
}

int HttpConnection::onMessageBegin() {
    m_request.clear();
    return 0;
}

int HttpConnection::onUrl(const char *at, size_t length) {
    m_request.url.append(at, length);
    return 0;
}

int HttpConnection::onHeader(const string &field, const string &value) {
    if (m_parser.nread >= ms_maxHeaderSize) {
        m_errorCode = 413;
        return -1;
    }
    m_request.headers.insert(make_pair(field, value));
    return 0;
}

int HttpConnection::onHeadersComplete() {
    m_request.majorVersion = m_parser.http_major;
    m_request.minorVersion = m_parser.http_minor;
    m_request.method = (http_method)m_parser.method;
    m_request.parseUrl();
    return 0;
}

int HttpConnection::onBody(const char *at, size_t length) {
    if (m_request.body.size() + length > ms_maxBodySize) {
        m_errorCode = 413;
        return -1;
    }
    m_request.body.append(at, length);
    return 0;
}

int HttpConnection::onMessageComplete() {
    if (!m_parser.upgrade) {
        m_callback(shared_from_this(), m_request, REQUEST_COMPLETE, NULL);
    }
    return 0;
}

int HttpConnection::onUpgrade(const char *at, size_t length) {
    string str(at, length);
    m_callback(shared_from_this(), m_request, REQUEST_UPGRADE, &str);
    return 0;
}

int HttpConnection::onError(const HttpError &error) {
    m_callback(shared_from_this(), m_request, REQUEST_ERROR, &error);
    return 0;
}

void HttpConnection::onConnectEvent(const shared_ptr<Connection> &con,
    CONNECT_EVENT event, const void *context) {
    switch (event) {
        case READ: {
            const string *str = (const string*)context;
            execute(str->data(), str->size());
            break;
        }
        case WRITE_COMPLETE: {
            m_sendCallback();
            m_sendCallback = bind(&dummyCallback);
            break;
        }
        default:
            break;
    }
}

LNET_NAMESPACE_END
