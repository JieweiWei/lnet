/*
 * ==================================================================================
 *
 *          Filename: httputil.h
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

#ifndef __LNET_HTTPUTIL_H__
#define __LNET_HTTPUTIL_H__

#include "lnethttp.h"

#include <string>

LNET_NAMESPACE_BEGIN

class HttpUtil {
public:
    static const std::string& codeMessage(int code);
    static const char* method2String(uint8_t method);
    static std::string escape(const std::string &str);
    static std::string unescape(const std::string &str);
    static std::string normalizeHeader(const std::string &str);

}; /* end of class HttpUtil */

LNET_NAMESPACE_END

#endif /* end of define __LNET_HTTPUTIL_H__ */
