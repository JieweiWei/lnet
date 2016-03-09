/*
 * ==================================================================================
 *
 *          Filename: poller.h
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

#ifndef __LNET_POLLER_H__
#define __LNET_POLLER_H__

#include "lnet.h"

#include <vector>

extern "C" {
    struct epoll_event;
}

LNET_NAMESPACE_BEGIN

struct IOEvent;

class Poller {
public:
    Poller(IOLoop *loop);
    ~Poller();

    int poll(int timeout, const std::vector<IOEvent*> &ioevents);

    int add(int fd, int events);
    int mod(int fd, int events);
    int del(int fd);

private:
    IOLoop *m_loop;
    int m_epollFd;
    struct epoll_event *m_events;
    size_t m_size;

}; /* end of class Poller */

LNET_NAMESPACE_END

#endif /* end of define __LNET_POLLER_H__ */
