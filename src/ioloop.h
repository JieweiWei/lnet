/*
 * ==================================================================================
 *
 *          Filename: ioloop.h
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

#ifndef __LNET_IOLOOP_H__
#define __LNET_IOLOOP_H__

#include "lnet.h"

#include "spinlock.h"

#include <vector>

LNET_NAMESPACE_BEGIN

class IOEvent;
class Poller;

class IOLoop : std::enable_shared_from_this<IOLoop> {
public:
    IOLoop();
    ~IOLoop();

    void start();
    void stop();

    int addHandler(int fd, int events, const IOHandler &handler);
    int updateHandler(int fd, int events);
    int removeHandler(int fd);

    std::shared_ptr<Timer> runAfter(int after, const Callback &callback);
    void addCallback(const Callback &callback);
    void runInWheel(int timeout, const TimingWheelHandler &handler);

private:
    void run();
    void onWake(const std::shared_ptr<Notifier> &notifier);
    void handleCallbacks();

private:
    int m_epollFd;
    bool m_running;
    std::vector<IOEvent*> m_events;
    Poller *m_poller;
    std::vector<Callback> m_callbacks;
    std::shared_ptr<Notifier> m_notifier;
    std::shared_ptr<TimingWheel> m_wheel;
    SpinLock m_lock;

}; /* end of class IOLoop */

LNET_NAMESPACE_END

#endif /* end of define __LNET_IOLOOP_H__ */
