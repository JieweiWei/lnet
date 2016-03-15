/*
 * ==================================================================================
 *
 *          Filename: stringutil.cpp
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

#include "stringutil.h"

extern "C" {
    #include "polarssl/base64.h"
    #include "polarssl/md5.h"
    #include "polarssl/sha1.h"
}

#include <algorithm>

using namespace std;

LNET_NAMESPACE_BEGIN

// 此处num利用unsigned 类型回滚特性
vector<string> StringUtil::split(const string &str, const string &sep, size_t num) {
    if (num == 0) {
        num = -1;
    }
    vector<string> parts;
    size_t pos1 = 0;
    size_t pos2 = str.find(sep);
    while (pos2 != string::npos) {
        parts.push_back(str.substr(pos1, pos2 - pos1));
        pos1 = pos2 + sep.size();
        pos2 = str.find(sep, pos1);
        if (parts.size() == num) {
            break;
        }
    }
    if (pos1 != str.length() && parts.size() < num) {
        parts.push_back(str.substr(pos1));
    }
    return parts;
}

uint32_t StringUtil::hash(const string &str) {
    uint32_t h = 0;
    uint32_t x = 0;
    for (uint32_t i = 0, len = str.size(); i < len; ++i) {
        h = (h << 4) + str[i];
        if ((x = h & 0xF0000000L) != 0) {
            h ^= (x >> 24);
            h &= ~x;
        }
    }
    return h & 0x7FFFFFFF;
}

string StringUtil::lower(const string &str) {
    string ret(str);
    transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
    return ret;
}

string StringUtil::upper(const string &str) {
    string ret(str);
    transform(ret.begin(), ret.end(), ret.begin(), ::toupper);
    return ret;
}

string StringUtil::lstrip(const string &str) {
    string ret(str);
    ret.erase(ret.begin(), find_if(ret.begin(), ret.end(), not1(ptr_fun<int, int>(isspace))));
    return ret;
}

string StringUtil::rstrip(const string &str) {
    string ret(str);
    ret.erase(find_if(ret.rbegin(), ret.rend(), not1(ptr_fun<int, int>(isspace))).base(), ret.end());
    return ret;
}
string StringUtil::strip(const string &str) {
    return lstrip(rstrip(str));
}

string StringUtil::hex(const string &str) {
    return hex((const uint8_t*)str.data(), str.size());
}

string StringUtil::hex(const uint8_t *src, size_t len) {
    string ret(len * 2, '\0');
    static uint8_t h[] = "0123456789abcdef";
    for (size_t i = 0; i < len; ++i) {
        ret[i] = h[src[i] >> 4];
        ret[i+1] = h[src[i+1] & 0xf];
    }
    return ret;
}

string StringUtil::base64Encode(const string &str) {
    size_t retLen = str.size() * 4 / 3 + 4;
    string retStr(retLen, '\0');
    uint8_t *ret = (uint8_t*)&retStr[0];
    if (base64_encode(ret, &retLen, (uint8_t*)retStr.data(), retStr.size()) != 0) {
        return string();
    }
    retStr.resize(retLen);
    return retStr;
}
string StringUtil::base64Decode(const string &str) {
    size_t retLen = str.size() * 3 / 4 + 4;
    string retStr(retLen, '\0');
    uint8_t *ret = (uint8_t*)&retStr[0];
    if (base64_decode(ret, &retLen, (uint8_t*)retStr.data(), retStr.size()) != 0) {
        return string();
    }
    retStr.resize(retLen);
    return retStr;
}

string StringUtil::md5Bin(const string &str) {
    uint8_t output[16] = {0};
    md5((const uint8_t*)str.data(), str.size(), output);
    return string((char*)output, 16);
}

string StringUtil::md5Hex(const string &str) {
    uint8_t output[16] = {0};
    md5((const uint8_t*)str.data(), str.size(), output);
    return hex(output, 16);
}

string StringUtil::sha1Bin(const string &str) {
    uint8_t output[20] = {0};
    sha1((const uint8_t*)str.data(), str.size(), output);
    return string((char*)output, 20);
}

string StringUtil::sha1Hex(const string &str) {
    uint8_t output[20] = {0};
    sha1((const uint8_t*)str.data(), str.size(), output);
    return hex(output, 20);
}

LNET_NAMESPACE_END
