/*
 * ==================================================================================
 *
 *          Filename: sockutil.h
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

#ifndef __LNET_SOCKUTIL_H__
#define __LNET_SOCKUTIL_H__

#include "lnet.h"

#include <string>

LNET_NAMESPACE_BEGIN

class Address;

class SockUtil {
public:
    static int createSocket();
    static int bindAndListen(const Address &addr);
    static int connect(int sockFd, const Address &addr);
    static int bindDevice(int sockFd, const std::string &device);

    static int setNoDelay(int sockFd, bool on = true);
    static int setReusable(int sockFd, bool on = true);
    static int setKeepAlive(int sockFd, bool on = true);

    static int getLocalAddr(int sockFd, Address &addr);
    static int getRemoteAddr(int sockFd, Address &addr);
    static int getSockError(int sockFd);
    static uint32_t getHostByName(const std::string &hostname);

}; /* end of class Sockutil */

LNET_NAMESPACE_END

#endif /* end of define __LNET_SOCKUTIL_H__ */
