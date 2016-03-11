/*
 * ==================================================================================
 *
 *          Filename: spinlock.h
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

#ifndef __LNET_SPINLOCK_H__
#define __LNET_SPINLOCK_H__

LNET_NAMESPACE_BEGIN

class SpinLock {
public:
    SpinLock() : m_lock(0) {
    }
    ~SpinLock() {
    }
    DISALLOW_COPY_AND_ASSIGN(SpinLock);

public:
    void lock() {
        while (__sync_lock_test_and_set(&m_lock, 1));
    }
    void unlock() {
        __sync_lock_release(&m_lock);
    }

private:
    volatile int m_lock;
}; /* end of class SpinLock */

LNET_NAMESPACE_END

#endif /* end of define __LNET_SPINLOCK_H__ */
