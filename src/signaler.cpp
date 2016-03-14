/*
 * ==================================================================================
 *
 *          Filename: signaler.cpp
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

#include "signaler.h"

#include "logger.h"
#include "ioloop.h"

#include <signal.h>
#include <sys/signalfd.h>
#include <unistd.h>
#include <assert.h>
#include <algorithm>

using namespace std;

LNET_NAMESPACE_BEGIN

Signaler::Signaler(const vector<int> &sigs, const SignalerHandler &handler)
    : m_loop(0)
    , m_running(false)
    , m_sigs(sigs)
    , m_handler(handler) {
    m_signalFd = createSignalFd(m_sigs);
}

Signaler::~Signaler() {
    if (m_signalFd >= 0) {
        close(m_signalFd);
    }

    LOG_INFO("destoryed signaler %d", m_signalFd);
}

void Signaler::start(IOLoop *loop) {
    assert(m_signalFd > 0);
    if (m_running) {
        LOG_WARN("signaler has started");
        return;
    }
    LOG_INFO("starting signaler %d", m_signalFd);
    m_running = true;
    m_loop = loop;
    m_loop->addHandler(
        m_signalFd,
        LNET_READ,
        bind(&Signaler::onSignal, this, _1, _2)
    );
}

void Signaler::stop() {
    assert(m_signalFd > 0);
    if (!m_running) {
        LOG_WARN("signaler has stopped");
        return;
    }
    LOG_INFO("stopping signaler %d", m_signalFd);
    m_running = false;
    m_loop->removeHandler(m_signalFd);
}

int Signaler::createSignalFd(const vector<int> &sigs) {
    sigset_t sigset;
    sigemptyset(&sigset);
    for (auto sig : sigs) {
        sigaddset(&sigset, sig);
    }
    if (sigprocmask(SIG_BLOCK, &sigset, NULL) < 0) {
        LOG_ERROR("sigprocmask error");
        return -1;
    }
    int sfd = signalfd(-1, &sigset, SFD_NONBLOCK | SFD_CLOEXEC);
    if (sfd < 0) {
        LOG_ERROR("signalfd error %s", errorMsg(errno));
    }
    return sfd;
}

void Signaler::onSignal(IOLoop*, int) {
    struct signalfd_siginfo sfdInfo;
    if (read(m_signalFd, &sfdInfo, sizeof(sfdInfo)) != sizeof(sfdInfo)) {
        LOG_ERROR("onSignal read error %s", errorMsg(errno));
        return;
    }
    int sig = sfdInfo.ssi_signo;
    if (find(m_sigs.begin(), m_sigs.end(), sig) == m_sigs.end()) {
        LOG_ERROR("unexcept sig %d", sig);
        return;
    }
    m_handler(shared_from_this(), sig);
}

LNET_NAMESPACE_END
