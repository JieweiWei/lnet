/*
 * ==================================================================================
 *
 *          Filename: logger.h
 *
 *       Description:
 *
 *            Verson: 1.0
 *           Created: 2016-03-06 09:18
 *          Compiler: g++
 *
 *            Author: Jiewei Wei <weijieweijerry@163.com>
 *           Company: Sun Yat-sen University
 *
 * ==================================================================================
 */

#ifndef __LNET_LOGGER_H__
#define __LNET_LOGGER_H__

#include "lnet.h"

#include "stdio.h"

LNET_NAMESPACE_BEGIN

class Logger {
public:
    enum Level {TRACE, DEBUG, INFO, WARN, ERROR, FATAL};

public:
    DISALLOW_COPY_AND_ASSIGN(Logger);
    Logger();
    Logger(const char *fileName);
    ~Logger();

    static Logger& getInstance() {
        static Logger logger;
        return logger;
    }

    void redirect(const char *fileName);

    void setLevel(Level level) { m_level = level; }
    Level getLevel() const { return m_level; }

    void trace(const char *file, const char *function, int line, const char *fmt, ...);
    void debug(const char *file, const char *function, int line, const char *fmt, ...);
    void info(const char *file, const char *function, int line, const char *fmt, ...);
    void warn(const char *file, const char *function, int line, const char *fmt, ...);
    void error(const char *file, const char *function, int line, const char *fmt, ...);
    void fatal(const char *file, const char *function, int line, const char *fmt, ...);

private:
    void log(const char *level, const char *file, const char *function, int line, const char *msg);

private:
    FILE *m_fd;
    /* 日志最小输出level */
    Level m_level;

}; /* end of class Logger */

#define LOG_SETFILE(fileName) Logger::getInstance().redirect(fileName)
#define LOG_SETLEVEL(level) Logger::getInstance().setLevel(level)
#define LOG_TRACE(fmt, args...) Logger::getInstance().trace(__FILE__, __FUNCTION__, __LINE__, fmt, ##args)
#define LOG_DEBUG(fmt, args...) Logger::getInstance().debug(__FILE__, __FUNCTION__, __LINE__, fmt, ##args)
#define LOG_INFO(fmt, args...) Logger::getInstance().info(__FILE__, __FUNCTION__, __LINE__, fmt, ##args)
#define LOG_WARN(fmt, args...) Logger::getInstance().warn(__FILE__, __FUNCTION__, __LINE__, fmt, ##args)
#define LOG_ERROR(fmt, args...) Logger::getInstance().error(__FILE__, __FUNCTION__, __LINE__, fmt, ##args)
#define LOG_FATAL(fmt, args...) Logger::getInstance().fatal(__FILE__, __FUNCTION__, __LINE__, fmt, ##args)

LNET_NAMESPACE_END

#endif /* end of define __LNET_LOGGER_H__ */
