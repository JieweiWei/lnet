/*
 * ==================================================================================
 *
 *          Filename: httputil.cpp
 *
 *       Description:
 *
 *            Verson: 1.0
 *           Created: 2016-03-12
 *          Compiler: g++
 *
 *            Author: Jiewei Wei <weijieweijerry@163.com>
 *           Company: Sun Yat-sen University
 *
 * ==================================================================================
 */

#include "httputil.h"


#include "stringutil.h"

extern "C" {
    #include "http_parser.h"
}

#include <vector>

using namespace std;

LNET_NAMESPACE_BEGIN

class Message {
public:
    Message() {
        m_message.resize(600);
        m_message[100] = "Continue";
        m_message[101] = "Switching Protocols";
        m_message[200] = "OK";
        m_message[201] = "Created";
        m_message[202] = "Accepted";
        m_message[203] = "Non-Authoritative Information";
        m_message[204] = "No Content";
        m_message[205] = "Reset Content";
        m_message[206] = "Partial Content";
        m_message[300] = "Multiple Choices";
        m_message[301] = "Moved Permanently";
        m_message[302] = "Found";
        m_message[303] = "See Other";
        m_message[304] = "Not Modified";
        m_message[305] = "Use Proxy";
        m_message[307] = "Temporary Redirect";
        m_message[400] = "Bad Request";
        m_message[401] = "Unauthorized";
        m_message[402] = "Payment Required";
        m_message[403] = "Forbidden";
        m_message[404] = "Not Found";
        m_message[405] = "Method Not Allowed";
        m_message[406] = "Not Acceptable";
        m_message[407] = "Proxy Authentication Required";
        m_message[408] = "Request Time-out";
        m_message[409] = "Conflict";
        m_message[410] = "Gone";
        m_message[411] = "Length Required";
        m_message[412] = "Precondition Failed";
        m_message[413] = "Request Entity Too Large";
        m_message[414] = "Request-URI Too Large";
        m_message[415] = "Unsupported Media Type";
        m_message[416] = "Requested range not satisfiable";
        m_message[417] = "Expectation Failed";
        m_message[500] = "Internal Server Error";
        m_message[501] = "Not Implemented";
        m_message[502] = "Bad Gateway";
        m_message[503] = "Service Unavailable";
        m_message[504] = "Gateway Time-out";
        m_message[505] = "HTTP Version not supported";
    }
    const string& getMessage(int code) {
        if ((size_t)code >= m_message.size()) {
            return unknown;
        }
        return m_message[code].empty() ? unknown : m_message[code];
    }
private:
    vector<string> m_message;
    static string unknown;
}; /* end of class Message */

string Message::unknown = "Unknown Error";

static Message s_message;

//refer to http://www.codeguru.com/cpp/cpp/algorithms/strings/article.php/c12759/URI-Encoding-and-Decoding.htm
static const char s_hex2dec[256] = {
    /*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
    /* 0 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 1 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 2 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 3 */  0, 1, 2, 3,  4, 5, 6, 7,  8, 9,-1,-1, -1,-1,-1,-1,

    /* 4 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 5 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 6 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 7 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,

    /* 8 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 9 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* A */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* B */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,

    /* C */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* D */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* E */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* F */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1
};

static const char s_dec2hex[17] = "0123456789ABCDEF";

const char s_safe[256] = {
    /*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
    /* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,

    /* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    /* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
    /* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    /* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,

    /* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,

    /* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};


const string& HttpUtil::codeMessage(int code) {
    return s_message.getMessage(code);
}

const char* HttpUtil::method2String(uint8_t method) {
    return http_method_str((http_method)method);
}

string HttpUtil::escape(const std::string &str) {
    if (str.empty()) {
        return "";
    }
    string ret(str.size() * 3, '\0');
    size_t j = 0;
    for (size_t i = 0; i < str.size(); ++i) {
        if (s_safe[str[i]]) {
            ret[j++] = str[i];
        } else {
            ret[j++] = '%';
            ret[j++] = s_dec2hex[str[i] >> 4];
            ret[j++] = s_dec2hex[str[i] & 0x0F];
        }
    }
    ret.resize(j);
    return ret;
}

string HttpUtil::unescape(const std::string &str) {
    if (str.size() <= 2) {
        return str;
    }
    string ret(str.size(), '\0');
    size_t i = 0;
    size_t j = 0;
    while (i < str.size() - 2) {
        if (str[i] == '%') {
            char dec1 = s_hex2dec[str[i+1]];
            char dec2 = s_hex2dec[str[i+2]];
            if (dec1 != -1 && dec2 != -1) {
                ret[j++] = (dec1 << 4) + dec2;
                i += 3;
                continue;
            }
        }
        ret[j++] = str[i++];
    }
    while (i < str.size()) {
        ret[j++] = str[i++];
    }
    ret.resize(j);
    return ret;
}

string HttpUtil::normalizeHeader(const std::string &str) {
    if (str.empty()) {
        return str;
    }
    string ret = StringUtil::lower(str);
    ret[0] = ::toupper(ret[0]);
    for (size_t i = 1; i < ret.size(); ++i) {
        if (ret[i-1] == '-') {
            ret[i] = ::toupper(ret[i]);
        }
    }
    return ret;
}

LNET_NAMESPACE_END
