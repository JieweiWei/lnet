/*
 * ==================================================================================
 *
 *          Filename: httpparser.cpp
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

#include "httpparser.h"

#include "logger.h"
#include "httputil.h"

LNET_NAMESPACE_BEGIN

struct http_parser_settings ms_settings;

class HttpParserSettings {
public:
    HttpParserSettings();

    static int onMessageBegin(struct http_parser *parser);
    static int onUrl(struct http_parser *parser, const char *at, size_t length);
    static int onStatusComplete(struct http_parser *parser);
    static int onHeaderField(struct http_parser *parser, const char *at, size_t length);
    static int onHeaderValue(struct http_parser *parser, const char *at, size_t length);
    static int onHeadersComplete(struct http_parser *parser);
    static int onBody(struct http_parser *parser, const char *at, size_t length);
    static int onMessageComplete(struct http_parser *parser);
}; /* end of class HttpParserSettings */

HttpParserSettings::HttpParserSettings() {
    ms_settings.on_message_begin = &HttpParserSettings::onMessageBegin;
    ms_settings.on_url = &HttpParserSettings::onUrl;
    ms_settings.on_status_complete = &HttpParserSettings::onStatusComplete;
    ms_settings.on_header_field = &HttpParserSettings::onHeaderField;
    ms_settings.on_header_value = &HttpParserSettings::onHeaderValue;
    ms_settings.on_headers_complete = &HttpParserSettings::onHeadersComplete;
    ms_settings.on_body = &HttpParserSettings::onBody;
    ms_settings.on_message_complete = &HttpParserSettings::onMessageComplete;
}

#define HTTPPARSER_ON_PARSE(event, at, len) do {    \
    HttpParser *p = (HttpParser*)parser->data;      \
    return p->onParser(HttpParser::event, at, len); \
} while(0)

int HttpParserSettings::onMessageBegin(struct http_parser *parser) {
    HTTPPARSER_ON_PARSE(ParserMessageBegin, NULL, 0);
}

int HttpParserSettings::onUrl(struct http_parser *parser, const char *at, size_t length) {
    HTTPPARSER_ON_PARSE(ParserUrl, at, length);
}

int HttpParserSettings::onStatusComplete(struct http_parser *parser) {
    HTTPPARSER_ON_PARSE(ParserStatusComplete, NULL, 0);
}

int HttpParserSettings::onHeaderField(struct http_parser *parser, const char *at, size_t length) {
    HTTPPARSER_ON_PARSE(ParserHeaderField, at, length);
}

int HttpParserSettings::onHeaderValue(struct http_parser *parser, const char *at, size_t length) {
    HTTPPARSER_ON_PARSE(ParserHeaderValue, at, length);
}

int HttpParserSettings::onHeadersComplete(struct http_parser *parser) {
    HTTPPARSER_ON_PARSE(ParserHeadersComplete, NULL, 0);
}

int HttpParserSettings::onBody(struct http_parser *parser, const char *at, size_t length) {
    HTTPPARSER_ON_PARSE(ParserBody, at, length);
}

int HttpParserSettings::onMessageComplete(struct http_parser *parser) {
    HTTPPARSER_ON_PARSE(ParserMessageComplete, NULL, 0);
}

static HttpParserSettings s_initHttpParserSetting;

HttpParser::HttpParser(enum http_parser_type type) {
    http_parser_init(&m_parser, type);
    m_parser.data = this;
    m_lastWasValue = true;
}

HttpParser::~HttpParser() {
}

int HttpParser::execute(const char *buf, size_t count) {
    int n = http_parser_execute(&m_parser, &ms_settings, buf, count);
    if (m_parser.upgrade) {
        onUpgrade(buf + n, count - n);
        return 0;
    } else if ((size_t)n != count) {
        int code = m_errorCode != 0 ? m_errorCode : 400;
        HttpError error(code, http_errno_description((http_errno)m_parser.http_errno));
        LOG_ERROR("parser error %s", error.message.c_str());
        onError(error);
        return code;
    }
    return 0;
}

int HttpParser::onParser(Event event, const char *at, size_t length) {
    switch (event) {
        case ParserMessageBegin:
            return handleMessageBegin();
        case ParserUrl:
            return onUrl(at, length);
        case ParserStatusComplete:
            return 0;
        case ParserHeaderField:
            return handleHeaderField(at, length);
        case ParserHeaderValue:
            return handleHeaderValue(at, length);
        case ParserHeadersComplete:
            return handleHeadersComplete();
        case ParserBody:
            return onBody(at, length);
        case ParserMessageComplete:
            return onMessageComplete();
        default:
            LOG_ERROR("unknown event %d", event);
            break;
    }
    return 0;
}

int HttpParser::handleMessageBegin() {
    m_field.clear();
    m_value.clear();
    m_lastWasValue = true;
    m_errorCode = 0;
    return onMessageBegin();
}

int HttpParser::handleHeaderField(const char* at, size_t length) {
    if (m_lastWasValue) {
        if (!m_field.empty()) {
            onHeader(HttpUtil::normalizeHeader(m_field), m_value);
        }
        m_field.clear();
        m_value.clear();
    }
    m_field.append(at, length);
    m_lastWasValue = false;
    return 0;
}

int HttpParser::handleHeaderValue(const char* at, size_t length) {
    m_value.append(at, length);
    m_lastWasValue = true;
    return 0;
}

int HttpParser::handleHeadersComplete() {
    if (!m_field.empty()) {
        onHeader(HttpUtil::normalizeHeader(m_field), m_value);
    }
    return onHeadersComplete();
}

LNET_NAMESPACE_END
