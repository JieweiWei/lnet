/*
 * ==================================================================================
 *
 *          Filename: timingwheel.cpp
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


#include "timingwheel.h"

#include "logger.h"
#include "timer.h"

using namespace std;

LNET_NAMESPACE_BEGIN

static void dummyTimingWheelHandler(const shared_ptr<TimingWheel>&) {
}

TimingWheel::TimingWheel(int interval, int maxBuckets) 
    : m_loop(0)
    , m_interval(interval)
    , m_maxBuckets(maxBuckets)
    , m_running(false)
    , m_nextBucket(0) {
    m_timer = make_shared<Timer>(bind(&TimingWheel::onTime, shared_from_this(), _1), interval, 0);
    m_buckets.resize(maxBuckets);
}

TimingWheel::~TimingWheel() {
}

void TimingWheel::start(IOLoop *loop) {
    if (m_running) {
        LOG_WARN("timingwheel has started");
        return;
    }
    m_loop = loop;
    m_running = true;
    m_timer->start(loop);
}

void TimingWheel::stop() {
    if (!m_running) {
        LOG_WARN("timingwheel has stopped");
        return;
    }
    m_running = false;
    m_timer->stop();
}

union Slot {
    uint64_t bucketAndSize;
    uint32_t parts[2];
};

uint64_t TimingWheel::add(const TimingWheelHandler &handler, int timeout) {
    int maxTimeout = m_interval * m_maxBuckets;
    if (timeout > maxTimeout) {
        LOG_ERROR("timeout %d > max %d", timeout, maxTimeout);
        return -1;
    }
    uint32_t bucket = (m_nextBucket + timeout / m_interval) % m_maxBuckets;
    m_buckets[bucket].push_back(handler);
    uint32_t size = m_buckets[bucket].size();
    Slot slot;
    slot.parts[0] = bucket;
    slot.parts[1] = size;
    return slot.bucketAndSize;
}

uint64_t TimingWheel::update(uint64_t bucketAndSize, int timeout) {
    Slot slot;
    slot.bucketAndSize = bucketAndSize;
    uint32_t bucket = slot.parts[0];
    uint32_t size = slot.parts[1];
    if (bucket >= (uint32_t)m_maxBuckets) {
        LOG_ERROR("bucket %u >= max %d", bucket, m_maxBuckets);
        return -1;
    }
    TimingChan &chan = m_buckets[bucket];
    if (size >= (uint32_t)(chan.size())) {
        LOG_ERROR("size %u >= chan size %u", size, chan.size());
        return -1;
    }
    // ??? handler move
    TimingWheelHandler handler = chan[size];
    chan[size] = bind(&dummyTimingWheelHandler, _1);
    return add(handler, timeout);
}

void TimingWheel::remove(uint64_t bucketAndSize) {
    Slot slot;
    slot.bucketAndSize = bucketAndSize;
    uint32_t bucket = slot.parts[0];
    uint32_t size = slot.parts[1];
    if (bucket >= (uint32_t)m_maxBuckets) {
        LOG_ERROR("bucket %u >= max %d", bucket, m_maxBuckets);
        return;
    }
    TimingChan &chan = m_buckets[bucket];
    if (size >= (uint32_t)(chan.size())) {
        LOG_ERROR("size %u >= chan size %u", size, chan.size());
        return;
    }
    chan[size] = bind(&dummyTimingWheelHandler, _1);
}

void TimingWheel::onTime(const std::shared_ptr<Timer> &timer) {
    int index = m_nextBucket;
    TimingChan &chan = m_buckets[index];
    for (auto handler : chan) {
        handler(shared_from_this());
    }
    chan.clear();
    m_nextBucket = (m_nextBucket + 1) % m_maxBuckets;
}

LNET_NAMESPACE_END
