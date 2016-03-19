/*
 * ==================================================================================
 *
 *          Filename: connection.cpp
 *
 *       Description:
 *
 *            Verson: 1.0
 *           Created: 2016-03-09
 *          Compiler: g++
 *
 *            Author: Jiewei Wei <weijieweijerry@163.com>
 *           Company: Sun Yat-sen University
 *
 * ==================================================================================
 */

#include "connection.h"

#include "logger.h"
#include "sockutil.h"
#include "ioloop.h"

#include <assert.h>
#include <unistd.h>
#include <sys/uio.h>

using namespace std;

LNET_NAMESPACE_BEGIN

static void dummyCallback(const shared_ptr<Connection>&, CONNECT_EVENT, const void*) {
}

static const int MaxReadBufSize = 4096;

Connection::Connection(IOLoop *loop, int sockFd)
    : m_loop(loop)
    , m_sockFd(sockFd)
    , m_status(None) {
    assert(m_sockFd > 0);
    m_callback = bind(&dummyCallback, _1, _2, _3);
}

Connection::~Connection() {
    LOG_INFO("connection destroyed %d", m_sockFd);
}

void Connection::clearEventCallback() {
    m_callback = bind(&dummyCallback, _1, _2, _3);
}

void Connection::shutdown(int after) {
    if (m_status == DisConnecting || m_status == Disconnected) {
        return;
    }
    m_status = DisConnecting;
    if (after == 0) {
        handleClose();
    } else {
        // here shared_from_this ???
        m_loop->runInWheel(
            after,
            bind(&Connection::handleClose, shared_from_this())
        );
    }
}

int Connection::send(const std::string &data) {
    if (!isConnected()) {
        LOG_ERROR("send error");
        return -1;
    }
    handleWrite(data);
    return 0;
}

void Connection::onEstablished() {
    if (m_status != None) {
        LOG_ERROR("invalid status %d != None", m_status);
        return;
    }
    m_status = Connected;
    updateActiveTime();
    shared_ptr<Connection> con = shared_from_this();
    m_loop->addHandler(
        m_sockFd,
        LNET_READ,
        bind(&Connection::onHandler, con, _1, _2)
    );
    m_callback(con, ESTABLISHED, NULL);
}

void Connection::connect(const Address &addr) {
    if (m_status != None) {
        LOG_ERROR("invalid status %d != None", m_status);
        return;
    }
    int ret = SockUtil::connect(m_sockFd, addr);
    if (ret < 0) {
        if (ret == EINPROGRESS) {
            m_status = Connecting;
        } else {
            handleError();
            return;
        }
    } else {
        m_status = Connected;
    }
    updateActiveTime();
    shared_ptr<Connection> con = shared_from_this();
    m_loop->addHandler(
        m_sockFd,
        isConnected() ? LNET_READ : LNET_WRITE,
        bind(&Connection::onHandler, con, _1, _2)
    );

    m_callback(con, CONNECT, NULL);
}

void Connection::onHandler(IOLoop*, int event) {
    shared_ptr<Connection> con = shared_from_this();
    if (event & LNET_READ) {
        handleRead();
    }
    if (event & LNET_WRITE) {
        if (isConnecting()) {
            handleConnect();
        } else {
            handleWrite();
        }
    }
    if (event & LNET_ERROR) {
        handleError();
    }
}

void Connection::handleRead() {
    if (!isConnected()) {
        return;
    }
    char buf[MaxReadBufSize] = {'\0'};
    int n = read(m_sockFd, buf, sizeof(buf));
    if (n > 0) {
        string str(buf, n);
        updateActiveTime();
        m_callback(shared_from_this(), READ, &str);
    } else if (n == 0) {
        handleClose();
    } else {
        int err = errno;
        if (err == EAGAIN || err == EWOULDBLOCK) {
            LOG_ERROR("read error %s", errorMsg(err));
            return;
        }
        handleError();
    }
}

void Connection::handleWrite(const std::string &data) {
    if (!isConnected()) {
        return;
    }
    if (m_sendBuf.empty() && data.empty()) {
        m_loop->updateHandler(m_sockFd, LNET_READ);
        return;
    }
    size_t totalSize = m_sendBuf.size() + data.size();
    struct iovec iov[2];
    iov[0].iov_base = (void*)m_sendBuf.data();
    iov[0].iov_len = m_sendBuf.size();
    iov[1].iov_base = (void*)data.data();
    iov[1].iov_len = data.size();
    ssize_t n = writev(m_sockFd, iov, 2);
    if ((size_t)n == totalSize) {
        m_sendBuf.clear();
        m_callback(shared_from_this(), WRITE_COMPLETE, NULL);
        m_loop->updateHandler(m_sockFd, LNET_READ);
        updateActiveTime();
    } else if (n < 0) {
        int err = errno;
        LOG_INFO("write error %s", errorMsg(err));
        if (err == EAGAIN || err == EWOULDBLOCK) {
            m_sendBuf.append(data);
            m_loop->updateHandler(m_sockFd, LNET_READ | LNET_WRITE);
        } else {
            // ???
            m_sendBuf.clear();
            handleError();
        }
    } else {
        if (m_sendBuf.size() < (size_t)n) {
            m_sendBuf = data.substr(n - m_sendBuf.size());
        } else {
            m_sendBuf = m_sendBuf.substr(n) + data;
        }
        updateActiveTime();
        m_loop->updateHandler(m_sockFd, LNET_READ | LNET_WRITE);
    }
}

void Connection::handleError() {
    disconnect();
    m_callback(shared_from_this(), ERROR, NULL);
}

void Connection::handleClose() {
    if (disconnect()) {
        m_callback(shared_from_this(), CLOSE, NULL);
    }
}

void Connection::handleConnect() {
    if (!isConnected()) {
        return;
    }
    if (SockUtil::getSockError(m_sockFd) != 0) {
        handleError();
        return;
    }
    // ???
    m_loop->updateHandler(m_sockFd, LNET_READ);
    updateActiveTime();
    m_status = Connected;
    m_callback(shared_from_this(), CONNECT, NULL);
}

void Connection::updateActiveTime() {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    m_lastActiveTime = time.tv_sec;
}

bool Connection::disconnect() {
    if (m_status == Disconnected) {
        return false;
    }
    m_status = Disconnected;
    m_loop->removeHandler(m_sockFd);
    close(m_sockFd);
    return true;
}


LNET_NAMESPACE_END

