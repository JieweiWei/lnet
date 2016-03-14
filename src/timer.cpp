/*
 * ==================================================================================
 *
 *          Filename: timer.cpp
 *
 *       Description:
 *
 *            Verson: 1.0
 *           Created: 2016-03-08
 *          Compiler: g++
 *
 *            Author: Jiewei Wei <weijieweijerry@163.com>
 *           Company: Sun Yat-sen University
 *
 * ==================================================================================
 */

#include "timer.h"

#include "logger.h"
#include "ioloop.h"

#include <sys/timerfd.h>
#include <unistd.h>
#include <assert.h>

using namespace std;

LNET_NAMESPACE_BEGIN

Timer::Timer(const TimerHandler &handler, int repeat, int after)
    : m_loop(0)
    , m_running(false)
    , m_repeated(false)
    , m_handler(handler) {
    m_timerFd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (m_timerFd < 0) {
        LOG_ERROR("timer create error %s", errorMsg(errno));
        return;
    }
    reset(repeat, after);
}

Timer::~Timer() {
    if (m_timerFd > 0) {
        close(m_timerFd);
    }
    LOG_INFO("destroyed %d", m_timerFd);
}

void Timer::start(IOLoop *loop) {
    assert(m_timerFd > 0);
    if (m_running) {
        LOG_WARN("timer has started");
        return;
    }
    LOG_INFO("starting timer %d", m_timerFd);
    m_loop = loop;
    m_running = true;
    m_loop->addHandler(
        m_timerFd,
        LNET_READ,
        bind(&Timer::onTimer, shared_from_this(), _1, _2)
    );
}

void Timer::stop() {
    if (!m_running) {
        LOG_WARN("timer has stopped");
        return;
    }
    m_running = false;
    m_loop->removeHandler(m_timerFd);
}

void Timer::reset(int repeat, int after) {
    static const uint64_t milliPerSec  = 1e3;
    //static const uint64_t microPerSec  = 1e6;
    //static const uint64_t nanoPerSec   = 1e9;
    static const uint64_t nanoPerMilli = 1e6;

    if (m_timerFd <= 0) {
        return;
    }
    m_repeated = repeat > 0;
    struct itimerspec t;
    if (m_repeated) {
        t.it_interval.tv_sec = (uint64_t)repeat / milliPerSec;
        t.it_interval.tv_nsec = ((uint64_t)repeat % milliPerSec) * nanoPerMilli;
    }
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    t.it_value.tv_sec = now.tv_sec + (uint64_t)after / milliPerSec;
    t.it_value.tv_nsec = now.tv_nsec + ((uint64_t)after % milliPerSec) * nanoPerMilli;
    if (timerfd_settime(m_timerFd, TFD_TIMER_ABSTIME, &t, NULL) < 0) {
        LOG_ERROR("set timer error");
    }
}

void Timer::onTimer(IOLoop*, int) {
    uint64_t exp;
    if (read(m_timerFd, &exp, sizeof(exp)) != sizeof(exp)) {
        LOG_ERROR("onTimer read error");
    } else {
        shared_ptr<Timer> timer = shared_from_this();
        m_handler(timer);
        if (!isRepeated()) {
            timer->stop();
        }
    }
}

LNET_NAMESPACE_END
