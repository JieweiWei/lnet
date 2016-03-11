/*
 * ==================================================================================
 *
 *          Filename: tcpserver.cpp
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

#include "tcpserver.h"

#include "ioloop.h"
#include "address.h"
#include "logger.h"
#include "acceptor.h"
#include "process.h"
#include "signaler.h"
#include "timingwheel.h"
#include "connection.h"

#include <algorithm>
#include <signal.h>

using namespace std;

LNET_NAMESPACE_BEGIN

static const int defaultIdleTimeout = 120;
static void dummyRunCallback(IOLoop*) {
}

TcpServer::TcpServer()
    : m_loop(0)
    , m_running(false)
    , m_maxIdleTimeout(defaultIdleTimeout) {
    m_process = make_shared<Process>();
    m_runCallback = bind(&dummyRunCallback, _1);
}

TcpServer::~TcpServer() {
    if (m_loop) {
        delete m_loop;
    }
}

int TcpServer::listen(const Address &addr, const ConnectionEventCallback &callback) {
    LOG_INFO("listening %s:%d", addr.ipstr().c_str(), addr.port());
    shared_ptr<Acceptor> acceptor = make_shared<Acceptor>(
        bind(&TcpServer::onNewConnection, this, _1, _2, callback)
    );
    if (acceptor->listen(addr) < 0) {
        return -1;
    }
    m_acceptors.push_back(acceptor);
    return 0;
}

void TcpServer::start(size_t maxProcess) {
    LOG_INFO("starting tcpserver");
    if (maxProcess > 1) {
        m_process->start(maxProcess, bind(&TcpServer::run, this));
    } else {
        run();
    }
}

void TcpServer::stop() {
    LOG_INFO("stopping tcpserver");
    m_process->stop();
    onStop();
}

void TcpServer::run() {
    if (m_running) {
        LOG_WARN("tcpserver is running");
        return;
    }
    m_loop = new IOLoop();
    m_running = true;
    m_loop->addCallback(bind(&TcpServer::onRun, this));
    m_loop->start();
}

void TcpServer::onRun() {
    LOG_INFO("tcpserver on run");
    for_each(
        m_acceptors.begin(),
        m_acceptors.end(),
        bind(&Acceptor::start, _1, m_loop)
    );
    m_signaler = make_shared<Signaler>(
        vector<int>({SIGINT, SIGTERM}),
        bind(&TcpServer::onSignal, this, _1, _2)
    );
    m_signaler->start(m_loop);
    m_idleWheel->start(m_loop);
    m_runCallback(m_loop);
}

void TcpServer::onStop() {
    LOG_INFO("tcpserver on stop");
    if (!m_running) {
        LOG_WARN("tcpserver has stopped");
        return;
    }
    m_running = false;
    m_idleWheel->stop();
    m_signaler->stop();
    for (auto acceptor : m_acceptors) {
        acceptor->stop();
    }
    m_loop->stop();
}

void TcpServer::onSignal(const std::shared_ptr<Signaler> &signaler, int sig) {
    switch (sig) {
        case SIGINT: case SIGTERM:
            onStop();
            break;
        default:
            LOG_ERROR("invalid signale %d", sig);
            break;
    }
}

void TcpServer::onIdleConnectCheck(const std::shared_ptr<TimingWheel> &wheel,
    const std::weak_ptr<Connection> &con) {
    // ???
    shared_ptr<Connection> c = con.lock();
    if (!c) {
        return;
    }
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    uint64_t now = time.tv_sec;
    if (now - c->lastActiveTime() > (uint64_t)m_maxIdleTimeout) {
        LOG_INFO("timeout, force shutdown");
        c->shutdown();
    } else {
        m_idleWheel->add(
            bind(&TcpServer::onIdleConnectCheck, this, _1, con),
            m_maxIdleTimeout * 900
        );
    }
}

void TcpServer::onNewConnection(IOLoop *loop, int fd,
    const ConnectionEventCallback &callback) {
    shared_ptr<Connection> con = make_shared<Connection>(loop, fd);
    con->setEventCallback(callback);
    con->onEstablished();
    // ???
    int after = m_maxIdleTimeout / 2 + random() % m_maxIdleTimeout;
    m_idleWheel->add(
        bind(&TcpServer::onIdleConnectCheck, this, _1, weak_ptr<Connection>(con)),
        after * 1000
    );
}

LNET_NAMESPACE_END
