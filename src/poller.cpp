/*
 * ==================================================================================
 *
 *          Filename: poller.cpp
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

#include "poller.h"

#include "logger.h"
#include "ioevent.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

LNET_NAMESPACE_BEGIN

static const int defaultEventSize = 1024;
static const int maxEventSize = 10240;

Poller::Poller(IOLoop *loop) : m_loop(loop), m_events(NULL) {
    m_epollFd = epoll_create1(EPOLL_CLOEXEC);
    if (m_epollFd < 0) {
        LOG_ERROR("epoll create error %s", errorMsg(errno));
    }
    m_size = defaultEventSize;
    m_events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * m_size);
    if (m_events == NULL) {
        LOG_ERROR("malloc events error %s", errorMsg(errno));
    }
}

Poller::~Poller() {
    if (m_epollFd > 0) {
        close(m_epollFd);
    }
    if (m_events != NULL) {
        free(m_events);
        m_events = NULL;
    }
}

#define EPOLL_WRITE (EPOLLOUT | EPOLLERR | EPOLLHUP)
#define EPOLL_READ (EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP)
#define EPOLL_EVENT_TO_LNET_EVENT(epollEvent)     \
    (epollEvent & EPOLL_WRITE ? LNET_WRITE : 0) | \
    (epollEvent & EPOLL_READ ? LNET_READ : 0)

#define LNET_EVENT_TO_EPOLL_EVENT(lnetEvent) \
    (lnetEvent & LNET_READ ? EPOLLIN : 0) |  \
    (lnetEvent & LNET_WRITE ? EPOLLOUT : 0)

int Poller::poll(int timeout, const std::vector<IOEvent*> &ioevents) {
    memset(m_events, 0, sizeof(struct epoll_event) * m_size);
    int num = epoll_wait(m_epollFd, m_events, m_size, timeout);
    if (num < 0) {
        LOG_ERROR("epoll_wait error %s", errorMsg(errno));
        return -1;
    }
    //LOG_DEBUG("epoll wait num %d", num);
    for (int i = 0; i < num; ++i) {
        struct epoll_event *epollEvent = m_events + i;
        int fd = epollEvent->data.fd;
        int real = EPOLL_EVENT_TO_LNET_EVENT(epollEvent->events);
        IOEvent *ioevent = (size_t)fd < ioevents.size() ? ioevents[fd] : 0;
        if (!ioevent) {
            continue;
        }
        int expect = ioevent->events;
        if (real & ~expect) {
            epollEvent->events = LNET_EVENT_TO_EPOLL_EVENT(expect);
            if (epoll_ctl(m_epollFd, expect ? EPOLL_CTL_MOD : EPOLL_CTL_DEL, fd, epollEvent) < 0) {
                LOG_ERROR("epoll_ctl error %s real:%d, expect:%d, fd:%d", errorMsg(errno), real, expect, fd);
                continue;
            }
        }
        // real -> expect ?
        (ioevent->handler)(m_loop, real);
    }
    if ((size_t)num == m_size && m_size != maxEventSize) {
        m_size *= 2;
        if (m_size > maxEventSize) {
            m_size = maxEventSize;
        }
        m_events = (struct epoll_event*)realloc(m_events, sizeof(struct epoll_event) * m_size);
        if (m_events == NULL) {
            LOG_ERROR("realloc error %s, size %d", errorMsg(errno), m_size);
        }
    }
    return num;
}


int Poller::add(int fd, int events) {
    assert(fd > 0);
    struct epoll_event epollEvent;
    epollEvent.data.u64 = 0;
    epollEvent.data.fd = fd;
    epollEvent.events = LNET_EVENT_TO_EPOLL_EVENT(events);
    if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &epollEvent) < 0) {
        LOG_ERROR("epoll_ctl add error %s", errorMsg(errno));
        return -1;
    }
    return 0;
}

int Poller::mod(int fd, int events) {
    assert(fd > 0);
    struct epoll_event epollEvent;
    epollEvent.data.fd = fd;
    epollEvent.data.u64 = 0;
    epollEvent.events = LNET_EVENT_TO_EPOLL_EVENT(events);
    if (epoll_ctl(m_epollFd, EPOLL_CTL_MOD, fd, &epollEvent) < 0) {
        LOG_ERROR("epoll_ctl mod error %s", errorMsg(errno));
        return -1;
    }
    return 0;

}
int Poller::del(int fd) {
    assert(fd > 0);
    if (epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, NULL) < 0) {
        LOG_ERROR("epoll_ctl del error %s", errorMsg(errno));
        return -1;
    }
    return 0;
}

LNET_NAMESPACE_END
