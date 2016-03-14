/*
 * ==================================================================================
 *
 *          Filename: tcpserver.h
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

#ifndef __LNET_TCPSERVER_H__
#define __LNET_TCPSERVER_H__

#include "lnet.h"

#include <vector>
#include <set>

LNET_NAMESPACE_BEGIN

class Address;
class Process;
class Acceptor;

class TcpServer {
public:
    TcpServer();
    ~TcpServer();
    DISALLOW_COPY_AND_ASSIGN(TcpServer);

    int listen(const Address &addr, const ConnectionEventCallback &callback);

    void start(size_t maxProcess = 0);
    void stop();

    IOLoop *loop() const { return m_loop; }

    void setRunCallback(const ServerRunCallback &callback) { m_runCallback = callback; }
    void setMaxIdleTimeout(int timeout) { m_maxIdleTimeout = timeout; }

private:
    void run();
    void onRun();
    void onStop();
    void onSignal(const std::shared_ptr<Signaler> &signaler, int sig);
    void onIdleConnectCheck(const std::shared_ptr<TimingWheel> &wheel,
        const std::weak_ptr<Connection> &con);
    void onNewConnection(IOLoop *loop, int fd, const ConnectionEventCallback &callback);

private:
    IOLoop *m_loop;
    bool m_running;
    std::shared_ptr<Process> m_process;
    std::vector<std::shared_ptr<Acceptor> > m_acceptors;
    std::shared_ptr<Signaler> m_signaler;
    int m_maxIdleTimeout;
    std::shared_ptr<TimingWheel> m_idleWheel;
    std::set<pid_t> m_workers;
    bool m_workerProc;
    ServerRunCallback m_runCallback;

}; /* end of class Tcpserver */

LNET_NAMESPACE_END

#endif /* end of define __LNET_TCPSERVER_H__ */
