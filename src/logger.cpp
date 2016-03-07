/*
 * ==================================================================================
 *
 *          Filename: logger.cpp
 *
 *       Description:
 *
 *            Verson: 1.0
 *           Created: 2016-03-06
 *          Compiler: g++
 *
 *            Author: Jiewei Wei <weijieweijerry@163.com>
 *           Company: Sun Yat-sen University
 *
 * ==================================================================================
 */

#include "logger.h"

#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

LNET_NAMESPACE_BEGIN

static const int MaxLogMsg = 1024;
static const char *DateTimeFormat = "%Y-%m-%d %H:%M:%s";

#define WRITE_LOG(level) do {                   \
    if (m_level <= level) {                     \
        char msg[MaxLogMsg] = {0};              \
        va_list ap;                             \
        va_start(ap, fmt);                      \
        vsnprintf(msg, sizeof(msg), fmt, ap);   \
        va_end(ap);                             \
        log(#level, file, function, line, msg); \
    }                                           \
} while(0)

Logger::Logger() : m_fd(stdout), m_level(TRACE) {}

Logger::Logger(const char *fileName) : m_level(TRACE) {
    m_fd = fopen(fileName, "ab+");
}

Logger::~Logger() {
    if (m_fd != stdout) {
        fclose(m_fd);
    }
}

void Logger::redirect(const char *fileName) {
    if (m_fd != stdout) {
        fclose(m_fd);
        m_fd = fopen(fileName, "ab+");
    }
}

void Logger::trace(const char *file, const char *function, int line, const char *fmt, ...) {
    WRITE_LOG(TRACE);
}

void Logger::debug(const char *file, const char *function, int line, const char *fmt, ...) {
    WRITE_LOG(DEBUG);
}

void Logger::info(const char *file, const char *function, int line, const char *fmt, ...) {
    WRITE_LOG(INFO);
}

void Logger::warn(const char *file, const char *function, int line, const char *fmt, ...) {
    WRITE_LOG(WARN);
}

void Logger::error(const char *file, const char *function, int line, const char *fmt, ...) {
    WRITE_LOG(ERROR);
}

void Logger::fatal(const char *file, const char *function, int line, const char *fmt, ...) {
    WRITE_LOG(FATAL);
}

void Logger::log(const char *level, const char *file, const char *function, int line, const char *msg) {
    char buf[64] = {0};
    time_t now = time(NULL);
    strftime(buf, sizeof(buf), DateTimeFormat, gmtime(&now));
    fprintf(m_fd, "%s %s [%d] %s %s:%d %s\n", buf, level, getpid(), function, file, line, msg);
}

__thread char errorMsgBuf[1024] = {0};

const char* errorMsg(int errorCode) {
    return strerror_r(errorCode, errorMsgBuf, sizeof(errorMsgBuf));
}

LNET_NAMESPACE_END
