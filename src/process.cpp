/*
 * ==================================================================================
 *
 *          Filename: process.cpp
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

#include "process.h"

#include "signaler.h"
#include "logger.h"

#include <signal.h>
#include <sys/wait.h>
#include <sys/signalfd.h>

using namespace std;

LNET_NAMESPACE_BEGIN

Process::Process() : m_running(false) {
    m_main = getpid();
    m_signalFd = Signaler::createSignalFd(vector<int>({SIGTERM}));
}

Process::~Process() {
}

void Process::start(size_t num, const ProcessCallback &callback) {
    m_running = true;
    for (size_t i = 0; i < num; ++i) {
        pid_t pid = create();
        if (pid > 0) {
            callback();
            return;
        }
    }
    while (!m_children.empty()) {
        int status = 0;
        pid_t pid;
        if ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            m_children.erase(pid);
            if (!m_running) {
                continue;
            }
            LOG_INFO("child %d was dead, restart it", pid);
            if (create() > 0) {
                callback();
                return;
            }
        } else {
            checkStop();
            sleep(1);
        }
    }
}

void Process::stop() {
    LOG_INFO("stopping child process");
    m_running = false;
    for (auto pid : m_children) {
        kill(pid, SIGTERM);
    }
}

pid_t Process::create() {
    pid_t pid = fork();
    if (pid < 0) {
        LOG_ERROR("fork error %s", errorMsg(errno));
        return pid;
    } else if (pid == 0) {
        m_children.clear();
        close(m_signalFd);
        return getpid();
    } else {
        m_children.insert(pid);
    }
    return 0;
}

void Process::checkStop() {
    struct signalfd_siginfo sfdInfo;
    if (read(m_signalFd, &sfdInfo, sizeof(sfdInfo)) != sizeof(sfdInfo)) {
        /* no signal */
        return;
    }
    int sig = sfdInfo.ssi_signo;
    switch (sig) {
        case SIGTERM:
            stop();
            break;
        default:
            LOG_INFO("get sig %d", sig);
            break;
    }
}

LNET_NAMESPACE_END
