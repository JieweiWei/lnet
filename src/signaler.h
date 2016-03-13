/*
 * ==================================================================================
 *
 *          Filename: signaler.h
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

#ifndef __LNET_SIGNALER_H__
#define __LNET_SIGNALER_H__

#include "lnet.h"

#include <vector>

LNET_NAMESPACE_BEGIN

class Signaler : public std::enable_shared_from_this<Signaler> {
public:
    Signaler(const std::vector<int> &sigs, const SignalerHandler &handler);
    DISALLOW_COPY_AND_ASSIGN(Signaler);
    ~Signaler();
    void start(IOLoop *loop);
    void stop();
    int sfd() const { return m_signalFd; }
    IOLoop* loop() const { return m_loop; }

public:
    static int createSignalFd(const std::vector<int> &sigs);

private:
    void onSignal(IOLoop*, int);

private:
    IOLoop *m_loop;
    int m_signalFd;
    bool m_running;
    std::vector<int> m_sigs;
    SignalerHandler m_handler;

}; /* end of class Signaler */

LNET_NAMESPACE_END

#endif /* end if define __LNET_SIGNALER_H__ */
