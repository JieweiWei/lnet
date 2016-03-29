/*
 * ==================================================================================
 *
 *          Filename: ioloop.cpp
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

#include "ioloop.h"

#include "poller.h"
#include "notifier.h"
#include "timingwheel.h"
#include "logger.h"
#include "ioevent.h"
#include "timer.h"

#include <signal.h>
#include <algorithm>

using namespace std;

LNET_NAMESPACE_BEGIN

static struct IgnoreSigPipe {
    IgnoreSigPipe() {
        signal(SIGPIPE, SIG_IGN);
    }
} s_ignoreSigPipe;

static const int DefaultEventsCapacity = 1024;
static const int MaxPollTimeout = 1000;

IOLoop::IOLoop()
    : m_running(false) {
    m_poller = new Poller(this);
    m_notifier = make_shared<Notifier>(bind(&IOLoop::onWake, this, _1));
    m_wheel = make_shared<TimingWheel>(1000, 3600);
    m_events.resize(DefaultEventsCapacity, NULL);
}

IOLoop::~IOLoop() {
    delete m_poller;
    for_each(m_events.begin(), m_events.end(), default_delete<IOEvent>());
}

void IOLoop::start() {
    if (m_running) {
        LOG_WARN("ioloop has started");
        return;
    }
    LOG_INFO("ioloop is starting");
    m_running = true;
    m_notifier->start(this);
    m_wheel->start(this);
    run();
}

void IOLoop::stop() {
    if (!m_running) {
        LOG_WARN("ioloop has stopped");
        return;
    }
    LOG_INFO("ioloop is stopping");
    m_running = false;
    m_notifier->notify();
}

int IOLoop::addHandler(int fd, int events, const IOHandler &handler) {
    if (m_events.size() <= (size_t)fd) {
        m_events.resize(fd + 1, NULL);
    }
    if (m_events[fd] != NULL) {
        LOG_ERROR("add duplicate handler %d", fd);
        return -1;
    }
    if (m_poller->add(fd, events) != 0) {
        return -1;
    }
    m_events[fd] = new IOEvent(fd, events, handler);
    //LOG_DEBUG("succ add event %d to fd %d", events, fd);
    return 0;
}

int IOLoop::updateHandler(int fd, int events) {
    if (m_events.size() <= (size_t)fd || m_events[fd] == NULL) {
        LOG_ERROR("invalid fd %d", fd);
        return -1;
    }
    if (m_events[fd]->events == events) {
        return 0;
    }
    if (m_poller->mod(fd, events) != 0) {
        return -1;
    }
    m_events[fd]->events = events;
    return 0;
}

int IOLoop::removeHandler(int fd) {
    if (m_events.size() <= (size_t)fd || m_events[fd] == NULL) {
        LOG_ERROR("invalid fd %d", fd);
        return -1;
    }
    m_poller->del(fd);
    delete m_events[fd];
    m_events[fd] = NULL;
    return 0;
}

static void onTimerHandler(const shared_ptr<Timer> timer, const Callback &callback) {
    callback();
    timer->stop();
}

shared_ptr<Timer> IOLoop::runAfter(int after, const Callback &callback) {
    shared_ptr<Timer> timer = make_shared<Timer>(
        bind(&onTimerHandler, _1, callback),
        after,
        0
    );
    timer->start(this);
    return timer;
}

void IOLoop::addCallback(const Callback &callback) {
    m_lock.lock();
    m_callbacks.push_back(callback);
    m_lock.unlock();
    m_notifier->notify();
}

void IOLoop::runInWheel(int timeout, const TimingWheelHandler &handler) {
    m_wheel->add(handler, timeout);
}

void IOLoop::run() {
    // key
    while (m_running) {
        m_poller->poll(MaxPollTimeout, m_events);
        handleCallbacks();
    }
    LOG_INFO("loop stop");
    m_notifier->stop();
}

void IOLoop::onWake(const std::shared_ptr<Notifier> &notifier) {
}

void IOLoop::handleCallbacks() {
    vector<Callback> callbacks;
    m_lock.lock();
    callbacks.swap(m_callbacks);
    m_lock.unlock();
    for (auto callback : callbacks) {
        callback();
    }
}

LNET_NAMESPACE_END
