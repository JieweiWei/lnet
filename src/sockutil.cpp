/*
 * ==================================================================================
 *
 *          Filename: sockutil.cpp
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

#include "sockutil.h"

#include "logger.h"
#include "address.h"

#include <sys/socket.h>
#include <error.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>

LNET_NAMESPACE_BEGIN

int SockUtil::createSocket() {
    return socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
}

int SockUtil::bindAndListen(const Address &addr) {
    int err = 0;
    int fd = createSocket();
    if (fd < 0) {
        err = errno;
        LOG_ERROR("create socket error %s", errorMsg(err));
        return fd;
    }
    SockUtil::setReusable(fd);
    do {
        struct sockaddr_in sockaddr = addr.sockaddr();
        if (bind(fd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
            err = errno;
            LOG_ERROR("bind address %s:%d error: %s", addr.ipstr().c_str(), addr.port(), errorMsg(err));
            break;
        }
        if (listen(fd, SOMAXCONN) < 0) {
            err = errno;
            LOG_ERROR("listen address %s:%d error: %s", addr.ipstr().c_str(), addr.port(), errorMsg(err));
            break;
        }
        return fd;
    } while (0);
    close(fd);
    return err;
}

int SockUtil::connect(int sockFd, const Address &addr) {
    struct sockaddr_in sockaddr = addr.sockaddr();
    int ret = ::connect(sockFd, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
    if (ret < 0) {
        return errno;
    }
    SockUtil::setNoDelay(sockFd);
    return ret;
}

int SockUtil::bindDevice(int sockFd, const std::string &device) {
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), device.c_str());
    struct sockaddr_in *sin = (struct sockaddr_in*)&ifr.ifr_addr;
    if (ioctl(sockFd, SIOCGIFADDR, &ifr) < 0) {
         LOG_ERROR("ioctl get addr error %s", errorMsg(errno));
         return -1;
    }
    sin->sin_family = AF_INET;
    sin->sin_port = 0;
    if (bind(sockFd, (struct sockaddr*)sin, sizeof(*sin)) < 0) {
        LOG_ERROR("bind interface error %s", errorMsg(errno));
        return -1;
    }
    return 0;
}

int SockUtil::setNoDelay(int sockFd, bool on) {
    int optvalue = on ? 1 : 0;
    return setsockopt(
        sockFd,
        IPPROTO_TCP,
        TCP_NODELAY,
        &optvalue,
        static_cast<socklen_t>(sizeof(optvalue))
    );
}

int SockUtil::setReusable(int sockFd, bool on) {
    int optvalue = on ? 1 : 0;
    return setsockopt(
        sockFd,
        SOL_SOCKET,
        SO_REUSEADDR,
        &optvalue,
        static_cast<socklen_t>(sizeof(optvalue))
    );
}

int SockUtil::setKeepAlive(int sockFd, bool on) {
    int optvalue = on ? 1 : 0;
    return setsockopt(
        sockFd,
        SOL_SOCKET,
        SO_KEEPALIVE,
        &optvalue,
        static_cast<socklen_t>(sizeof(optvalue))
    );
}

#define GET_ADDR(func) do {                                     \
    struct sockaddr_in sockaddr;                                \
    socklen_t len = sizeof(sockaddr);                           \
    if (func(sockFd, (struct sockaddr*)&sockaddr, &len) != 0) { \
        return errno;                                           \
    }                                                           \
    addr = Address(sockaddr);                                   \
} while (0)

int SockUtil::getLocalAddr(int sockFd, Address &addr) {
    GET_ADDR(getsockname);
    return 0;
}

int SockUtil::getRemoteAddr(int sockFd, Address &addr) {
    GET_ADDR(getpeername);
    return 0;
}

int SockUtil::getSockError(int sockFd) {
    int error = 0;
    socklen_t len = static_cast<socklen_t>(sizeof(error));
    if (getsockopt(sockFd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
        return errno;
    }
    return error;
}

// TODO: 使用系统调用gethostbyname
uint32_t SockUtil::getHostByName(const std::string &hostname) {
    struct addrinfo hint;
    struct addrinfo *ret;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(hostname.c_str(), NULL, &hint, &ret) != 0) {
        LOG_ERROR("getaddrinfo error %s", errorMsg(errno));
        return -1;
    }
    for (struct addrinfo *cur = ret; cur != NULL; cur = cur->ai_next) {
        return ((struct sockaddr_in*)(cur->ai_addr))->sin_addr.s_addr;
    }
    LOG_ERROR("getHostByName error");
    return -1;
}

LNET_NAMESPACE_END
