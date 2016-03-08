/*
 * ==================================================================================
 *
 *          Filename: acceptor.h
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

#ifndef __LNET_ACCEPTOR_H__
#define __LNET_ACCEPTOR_H__

#include "lnet.h"

LNET_NAMESPACE_BEGIN

class Address;

class Acceptor : public std::enable_shared_from_this<Acceptor> {
public:
    Acceptor(const NewConnectCallback &callback);
    ~Acceptor();
    DISALLOW_COPY_AND_ASSIGN(Acceptor);

    int listen(const Address &addr);

    void start(IOLoop *loop);
    void stop();

private:
    void onAccept(IOLoop* loop, int);

private:
    IOLoop *m_loop;
    int m_sockFd;
    bool m_running;
    NewConnectCallback m_callback;

}; /* end of class Acceptor */

LNET_NAMESPACE_END

#endif /* end of define __LNET_ACCEPTOR_H__ */
