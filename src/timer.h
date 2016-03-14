/*
 * ==================================================================================
 *
 *          Filename: timer.h
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

#ifndef __LNET_TIMER_H__
#define __LNET_TIMER_H__

#include "lnet.h"

LNET_NAMESPACE_BEGIN

class Timer : public std::enable_shared_from_this<Timer> {
public:
    // ms
    Timer(const TimerHandler &handler, int repeat, int after);
    ~Timer();
    DISALLOW_COPY_AND_ASSIGN(Timer);

    void start(IOLoop *loop);
    void stop();

    void reset(int repeat, int after);

    int fd() const { return m_timerFd; }

    IOLoop* loop() const { return m_loop; }

    bool isRepeated() const { return m_repeated; }

private:
    void onTimer(IOLoop*, int);

private:
    IOLoop* m_loop;
    bool m_running;
    int m_timerFd;
    bool m_repeated;
    TimerHandler m_handler;

}; /* end of class Timer */

LNET_NAMESPACE_END

#endif /* end of define __LNET_TIMER_H__ */
