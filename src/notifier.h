/*
 * ==================================================================================
 *
 *          Filename: notifier.h
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

#ifndef __LENT_NOTIFIER_H__
#define __LENT_NOTIFIER_H__

#include "lnet.h"

LNET_NAMESPACE_BEGIN

class Notifier : public std::enable_shared_from_this<Notifier> {
public:
    Notifier(const NotifierHandler &handler);
    ~Notifier();
    DISALLOW_COPY_AND_ASSIGN(Notifier);

    void start(IOLoop *loop);
    void stop();

    void notify();

    IOLoop* loop() { return m_loop; }

private:
    void onEvent(IOLoop*, int);

private:
    IOLoop *m_loop;
    int m_efd;
    bool m_running;
    NotifierHandler m_handler;

}; /* end of class Notifier */

LNET_NAMESPACE_END

#endif /* end of define __LENT_NOTIFIER_H__ */
