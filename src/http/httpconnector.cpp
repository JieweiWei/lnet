/*
 * ==================================================================================
 *
 *          Filename: httpconnector.cpp
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

#include "httpconnector.h"

#include "connector.inl"

using namespace std;

LNET_NAMESPACE_BEGIN

size_t HttpConnector::ms_maxHeaderSize = 80 * 1024;
size_t HttpConnector::ms_maxBodySize   = 10 * 1024 * 1024;

void dummyCallback(const shared_ptr<HttpConnector>&, const HttpResponse&, RESPONSE_EVENT) {
}

HttpConnector::HttpConnector()
    : HttpParser(HTTP_RESPONSE)
    , Connector<HttpConnector>() {
    m_callback = bind(&dummyCallback, _1, _2, _3);
}

HttpConnector::~HttpConnector() {
}

void HttpConnector::clearCallback() {
    m_callback = bind(&dummyCallback, _1, _2, _3);
}

void HttpConnector::handleRead(const char *buf, size_t count) {
    shared_ptr<HttpConnector> con = shared_from_this();
    execute(buf, count);
}

int HttpConnector::onMessageBegin() {
    m_response.clear();
    return 0;
}

int HttpConnector::onHeader(const std::string &field, const std::string &value) {
    if (m_parser.nread >= ms_maxHeaderSize) {
        m_errorCode = 413;
        return -1;
    }
    m_response.headers.insert(make_pair(field, value));
    return 0;
}

int HttpConnector::onHeadersComplete() {
    m_response.code = m_parser.status_code;
    return 0;
}

int HttpConnector::onBody(const char *at, size_t length) {
    if (m_response.body.size() + length >= ms_maxBodySize) {
        m_errorCode = 413;
        return -1;
    }
    m_response.body.append(at, length);
    return 0;
}

int HttpConnector::onMessageComplete() {
    m_callback(shared_from_this(), m_response, RESPONSE_COMPLETE);
    return 0;
}

int HttpConnector::onError(const HttpError &error) {
    HttpResponse response(error.code);
    response.body = error.message;
    m_callback(shared_from_this(), response, RESPONSE_ERROR);
    return 0;
}

LNET_NAMESPACE_END
