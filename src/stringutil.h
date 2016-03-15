/*
 * ==================================================================================
 *
 *          Filename: stringutil.h
 *
 *       Description:
 *
 *            Verson: 1.0
 *           Created: 2016-03-06
 *          Compiler: g++
 *
 *            Author: Jiewei Wei <weijieweijerry@163.com>
 *           Company: Sun Yat-sen University
 *
 * ==================================================================================
 */

#ifndef __LNET_STRINGUTIL_H__
#define __LNET_STRINGUTIL_H__

#include "lnet.h"

#include <vector>
#include <string>
#include <sstream>

LNET_NAMESPACE_BEGIN

class StringUtil {
public:
    static std::vector<std::string> split(const std::string &str, const std::string &sep, size_t num = size_t(-1));
    static uint32_t hash(const std::string &str);

    static std::string lower(const std::string &str);
    static std::string upper(const std::string &str);

    static std::string lstrip(const std::string &str);
    static std::string rstrip(const std::string &str);
    static std::string strip(const std::string &str);

    static std::string hex(const std::string &str);
    static std::string hex(const uint8_t *src, size_t len);

    static std::string base64Encode(const std::string &str);
    static std::string base64Decode(const std::string &str);

    static std::string md5Bin(const std::string &str);
    static std::string md5Hex(const std::string &str);

    static std::string sha1Bin(const std::string &str);
    static std::string sha1Hex(const std::string &str);


    template <typename T>
    static std::string toString(const T &t) {
        std::stringstream ss;
        ss << t;
        return ss.str();
    }
    static std::string toString(const char *t) {
        return std::string(t);
    }
    static std::string toString(const std::string &t) {
        return std::string(t);
    }

}; /* end of class StringUtil */

LNET_NAMESPACE_END

#endif /* end of define __LNET_STRINGUTIL_H__ */
