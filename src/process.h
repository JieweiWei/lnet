/*
 * ==================================================================================
 *
 *          Filename: process.h
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

#ifndef __LNET_PROCESS_H__
#define __LNET_PROCESS_H__

#include "lnet.h"

#include <unistd.h>
#include <set>

LNET_NAMESPACE_BEGIN

class Process {
public:
    Process();
    ~Process();

    void start(size_t num, const ProcessCallback &callback);
    void stop();

    bool isMain() const { return m_main == getpid(); }
    bool hasChildren() const { return m_children.size() > 0; }

private:
    pid_t create();
    void checkStop();

private:
    pid_t m_main;
    bool m_running;
    std::set<pid_t> m_children;
    int m_signalFd;

}; /* end of class Process */

LNET_NAMESPACE_END

#endif /* end of define __LNET_PROCESS_H__*/
