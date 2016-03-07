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

#include <sys/socket.h>
#include <error.h>
#include <unistd.h>
#include <netinet/tcp.h>

//#include <netinet/tcp.h>
//#include <assert.h>
//#include <netdb.h>
//#include <string.h>
//#include <sys/ioctl.h>
//#include <sys/types.h>
//#include <net/if.h>
//#include <netinet/in.h>

#include "logger.h"
#include "address.h"

LNET_NAMESPACE_BEGIN

int SockUtil::createSocket() {
    return socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
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

int bindDevice(int sockFd, const std::string &device);

int SockUtil::setNoDelay(int sockFd, bool on) {
    int optvalue = on;
    return setsockopt(
        sockFd,
        IPPROTO_TCP,
        TCP_NODELAY,
        &optvalue,
        static_cast<socklen_t>(sizeof(optvalue))
    );
}

int SockUtil::setReusable(int sockFd, bool on) {
    int optvalue = on;
    return setsockopt(
        sockFd,
        SOL_SOCKET,
        SO_REUSEADDR,
        &optvalue,
        static_cast<socklen_t>(sizeof(optvalue))
    );
}

int SockUtil::setKeepAlive(int sockFd, bool on) {
    int optvalue = on;
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
#undef GET_ADDR

int SockUtil::getSockError(int sockFd) {
    
}

uint32_t getHostByName(const std::string &host);


LNET_NAMESPACE_END
