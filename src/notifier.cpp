/*
 * ==================================================================================
 *
 *          Filename: notifier.cpp
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

#include "notifier.h"

#include "logger.h"
#include "ioloop.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <assert.h>

using namespace std;

LNET_NAMESPACE_BEGIN

Notifier::Notifier(const NotifierHandler &handler)
    : m_loop(NULL)
    , m_running(false)
    , m_handler(handler) {
    m_eventFd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (m_eventFd < 0) {
        LOG_ERROR("eventfd error %s", errorMsg(errno));
    }
}

Notifier::~Notifier() {
    if (m_eventFd > 0) {
        close(m_eventFd);
    }
    LOG_INFO("destoryed notifier %d", m_eventFd);
}

void Notifier::start(IOLoop *loop) {
    assert(m_eventFd > 0);
    if (m_running) {
        LOG_WARN("notifier has started");
        return;
    }
    LOG_INFO("starting notifier %d", m_eventFd);
    m_running = true;
    m_loop = loop;
    m_loop->addHandler(
        m_eventFd,
        LNET_READ,
        bind(&Notifier::onEvent, shared_from_this(), _1, _2)
    );
}

void Notifier::stop() {
    assert(m_eventFd > 0);
    if (!m_running) {
        LOG_WARN("notifier has stopped");
        return;
    }
    LOG_INFO("stopping notifier %d", m_eventFd);
    m_running = false;
    m_loop->removeHandler(m_eventFd);
}

void Notifier::notify() {
    eventfd_t val = 1;
    if (eventfd_write(m_eventFd, val) < 0) {
        LOG_ERROR("eventfd_write error");
    }
}

void Notifier::onEvent(IOLoop*, int) {
    eventfd_t val;
    if (eventfd_read(m_eventFd, &val) < 0) {
        LOG_ERROR("eventfd read error");
        return;
    }
    m_handler(shared_from_this());
}

LNET_NAMESPACE_END
