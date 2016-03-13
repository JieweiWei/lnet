/*
 * ==================================================================================
 *
 *          Filename: httpparser.h
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

#ifndef __LNET_HTTPPARSER_H__
#define __LNET_HTTPPARSER_H__

extern "C" {
    #include "http_parser.h"
}

#include "lnethttp.h"

LNET_NAMESPACE_BEGIN

class HttpParser {
public:
    friend class HttpParserSettings;
    enum Event {
        ParserMessageBegin,
        ParserUrl,
        ParserStatusComplete,
        ParserHeaderField,
        ParserHeaderValue,
        ParserHeadersComplete,
        ParserBody,
        ParserMessageComplete
    };

public:
    HttpParser(enum http_parser_type type);
    virtual ~HttpParser();
    DISALLOW_COPY_AND_ASSIGN(HttpParser);
    enum http_parser_type getType() { return (http_parser_type)m_parser.type; }
    int execute(const char *buf, size_t count);

protected:
    virtual int onMessageBegin() { return 0; }
    virtual int onUrl(const char*, size_t) { return 0; }
    virtual int onHeader(const std::string &field, const std::string &value) { return 0; }
    virtual int onHeadersComplete() { return 0; }
    virtual int onBody(const char*, size_t) { return 0; }
    virtual int onMessageComplete() { return 0; }
    virtual int onUpgrade(const char*, size_t) { return 0; }
    virtual int onError(const HttpError &error) {  return 0;}

private:
    int onParser(Event event, const char * at, size_t length);
    int handleMessageBegin();
    int handleHeaderField(const char *at, size_t length);
    int handleHeaderValue(const char *at, size_t length);
    int handleHeadersComplete();

protected:
    struct http_parser m_parser;
    std::string m_field;
    std::string m_value;
    bool m_lastWasValue;
    int m_errorCode;

}; /* end of class HttpParser */

LNET_NAMESPACE_END

#endif /* end of define __LNET_HTTPPARSER_H__ */
