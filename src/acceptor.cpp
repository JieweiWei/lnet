/*
 * ==================================================================================
 *
 *          Filename: acceptor.cpp
 *
 *       Description:
 *
 *            Verson: 1.0
 *           Created: 2016-03-07
 *          Compiler: g++
 *
 *            Author: Jiewei Wei <weijieweijerry@163.com>
 *           Company: Sun Yat-sen University
 *
 * ==================================================================================
 */

#include "acceptor.h"

#include "sockutil.h"
#include "logger.h"
#include "ioloop.h"

#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/socket.h>

using namespace std;

LNET_NAMESPACE_BEGIN

Acceptor::Acceptor(const NewConnectCallback &callback)
    : m_loop(NULL)
    , m_sockFd(0)
    , m_running(false)
    , m_callback(callback) {
}

Acceptor::~Acceptor() {
    if (m_sockFd > 0) {
        close(m_sockFd);
    }
}

int Acceptor::listen(const Address &addr) {
    int sockFd = SockUtil::bindAndListen(addr);
    if (sockFd >= 0) {
        m_sockFd = sockFd;
    }
    return sockFd;
}

void Acceptor::start(IOLoop *loop) {
    assert(m_sockFd > 0);
    if (m_running) {
        LOG_WARN("acceptor has started");
        return;
    }
    LOG_INFO("starting acceptor %d", m_sockFd);
    m_loop = loop;
    m_running = true;
    m_loop->addHandler(
        m_sockFd,
        LNET_READ,
        bind(&Acceptor::onAccept, this, _1, _2)
    );
}

void Acceptor::stop() {
    assert(m_sockFd > 0);
    if (!m_running) {
        LOG_WARN("acceptor has stopped");
        return;
    }
    LOG_INFO("stopping acceptor %d", m_sockFd);
    m_running = false;
    m_loop->removeHandler(m_sockFd);
}

void Acceptor::onAccept(IOLoop* loop, int) {
    int clientFd = accept4(m_sockFd, NULL, NULL, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (clientFd < 0) {
        int err = errno;
        if (err == EMFILE || err == ENFILE) {
            LOG_ERROR("accept error %s", errorMsg(err));
            clientFd = accept(m_sockFd, NULL, NULL);
            close(clientFd);
        }
    } else {
        LOG_INFO("accept %d", clientFd);
        SockUtil::setNoDelay(clientFd);
        m_callback(loop, clientFd);
    }
}

LNET_NAMESPACE_END
