/*
 * ==================================================================================
 *
 *          Filename: timingwheel.h
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


#ifndef __LNET_TIMINGWHEEL_H__
#define __LNET_TIMINGWHEEL_H__

#include "lnet.h"

#include <vector>

LNET_NAMESPACE_BEGIN

class TimingWheel : public std::enable_shared_from_this<TimingWheel> {
public:
    TimingWheel(int interval, int maxBuckets);
    ~TimingWheel();
    DISALLOW_COPY_AND_ASSIGN(TimingWheel);

    void start(IOLoop *loop);
    void stop();

    uint64_t add(const TimingWheelHandler &handler, int timeout);
    uint64_t update(uint64_t bucketAndSize, int timeout);
    void remove(uint64_t bucketAndSize);

private:
    void onTime(const std::shared_ptr<Timer> &timer);

private:
    IOLoop *m_loop;
    bool m_running;
    std::shared_ptr<Timer> m_timer;
    int m_interval;
    int m_maxBuckets;
    int m_nextBucket;
    using TimingChan = std::vector<TimingWheelHandler>;
    using Buckets = std::vector<TimingChan>;
    Buckets m_buckets;

}; /* end of class Timingwheel */

LNET_NAMESPACE_END

#endif /* end of define __LNET_TIMINGWHEEL_H__ */
