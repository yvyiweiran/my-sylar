#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__
#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <stdarg.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <set>
#include <typeinfo>

#include "singleton.h"
#include "thread.h"
#include "util.h"

#define SYLAR_LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        sylar::LogEventWrap( sylar::LogEvent::ptr(new sylar::LogEvent(\
        logger, level, __FILE__, __LINE__, 0, sylar::GetThreadId()\
        , sylar::GetFiderId(), time(0), sylar::Thread::GetName()))).getSS()

#define SYLAR_LOG_DEBUG(logger)SYLAR_LOG_LEVEL(logger, sylar::LogLevel::Level::DEBUG)
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::Level::INFO)
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::Level::WARN)
#define SYLAR_LOG_ERROR(logger)SYLAR_LOG_LEVEL(logger, sylar::LogLevel::Level::ERROR)
#define SYLAR_LOG_FATAL(logger)SYLAR_LOG_LEVEL(logger, sylar::LogLevel::Level::FATAL)

#define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...)\
    if(logger->getLevel() <= level) \
        sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(\
            logger, level, __FILE__, __LINE__, 0, sylar::GetThreadId()\
            , sylar::GetFiderId(), time(0), sylar::Thread::GetName())) \
        ).getEvent()->format(fmt, __VA_ARGS__)

#define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...)SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::Level::DEBUG, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_INFO(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::Level::INFO, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_WARN(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::Level::WARN, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_ERROR(logger, fmt, ...)SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::Level::ERROR, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_FATAL(logger, fmt, ...)SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::Level::FATAL, fmt, __VA_ARGS__)

#define SYLAR_LOG_ROOT() sylar::Singleton<sylar::LoggerManager>::GetIstance()->getRoot()
#define SYLAR_LOG_NAME(name) sylar::LoggerMgr::GetIstance()->getLogger(name)
namespace sylar {

class Logger;
class LoggerManager;
class LogEvent;
// 日志级别
class LogLevel {
public:
    enum Level {
        UNKNOW = 0,
        DEBUG  = 1,
        INFO   = 2,
        WARN   = 3,
        ERROR  = 4,
        FATAL  = 5
    };
    static const char *ToString( LogLevel::Level level );
    static LogLevel::Level ToLevel( std::string level);
};

class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
                , const char *file, int32_t line, uint32_t elapse
                ,uint32_t thread_id, uint32_t fiber_id, uint64_t time
                ,const std::string& threadname);

    const char*         getFile() const { return m_file; }
    int32_t             getLine() const { return m_line; }
    uint32_t            getElapse() const { return m_elapse; }
    uint32_t            getTheadId() const { return m_threadId; }
    uint32_t            getFiberId() const { return m_fiberId; }
    uint64_t            getTime() const { return m_time; }
    std::string         getContent() const { return m_ss.str(); }
    const std::string&  getThreadName() const { return m_threadname; }
    LogLevel::Level     getLevel() const {return m_level;}
    std::shared_ptr<Logger> getLogger() const {return m_logger;}

    std::stringstream&  getSS() { return m_ss; }
    void format(const char* fmt, ...);
    void format(const char* fmt, va_list al);
private:
    std::shared_ptr<Logger> m_logger;
    LogLevel::Level     m_level;
    const char*         m_file     = nullptr; // 文件名
    int32_t             m_line     = 0;       // 行号
    uint32_t            m_elapse   = 0; // 程序启动开始到现在的毫秒数
    uint32_t            m_threadId = 0; // 线程id
    uint32_t            m_fiberId  = 0; // 协程id
    uint64_t            m_time     = 0; // 时间戳
    std::string         m_threadname;
    std::stringstream   m_ss;
};

class LogEventWrap {
public:
    LogEventWrap(LogEvent::ptr e);
    ~LogEventWrap();
    std::stringstream& getSS();

    LogEvent::ptr getEvent() const {return m_event;}
private:
    LogEvent::ptr m_event;
};

// 日志格式器
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter( const std::string &pattern );
    std::string format( std::shared_ptr<Logger> logger, LogLevel::Level level,
                        LogEvent::ptr event );

public:
    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem() {}
        virtual void format( std::ostream &os, std::shared_ptr<Logger> logger,
                                LogLevel::Level level, LogEvent::ptr event ) = 0;
    };
    void init();
    bool isError() const { return m_error;}
    const std::string getPattern() const { return m_pattern;}
private:
    // 日志格式模板
    std::string m_pattern;
    // 日志解析后的格式
    std::vector<FormatItem::ptr> m_items;
    // 是否有错误
    bool m_error = false;
};

// 日志输出地
class LogAppender {
friend class Logger;
public:
    using MutexType = Spinlock;
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender(){};
    virtual std::string toYamlString() = 0;

    virtual void log( std::shared_ptr<Logger> logger, LogLevel::Level level,
                        LogEvent::ptr event ) = 0;
    void setFormatter( LogFormatter::ptr val );
    LogFormatter::ptr getFormatter();
    LogLevel::Level getLevel() const { return m_level;}
    void setLevel(LogLevel::Level val) {m_level = val;}
protected:
    LogLevel::Level   m_level = LogLevel::Level::DEBUG;
    LogFormatter::ptr m_formatter = nullptr;
    MutexType m_mutex;
    bool m_hasFormatter = false;
};

// 日志器
class Logger : public std::enable_shared_from_this<Logger> {
    friend class LoggerManager;
public:
    using MutexType = Spinlock;
    typedef std::shared_ptr<Logger> ptr;
    Logger( const std::string &name = "root" );

    void log( LogLevel::Level level, LogEvent::ptr &event );
    void debug( LogEvent::ptr event );
    void info( LogEvent::ptr event );
    void warn( LogEvent::ptr event );
    void error( LogEvent::ptr event );
    void fatal( LogEvent::ptr event );

    void addAppender( LogAppender::ptr appender );
    void delAppender( LogAppender::ptr appender );
    void clearAppender();
    void setLevel( LogLevel::Level level ) { m_level = level; }
    void setFormatter(const std::string& val);
    void setFormatter(const LogFormatter::ptr val);
    LogFormatter::ptr getFormatter();

    LogLevel::Level getLevel() const { return m_level; }
    const std::string &getName() const { return m_name; }

    std::string toYamlString();
private:
    std::string                 m_name;      // 日志名称
    LogLevel::Level             m_level;     // 日志级别
    std::list<LogAppender::ptr> m_appenders; // Appender集合
    LogFormatter::ptr           m_formatter;
    Logger::ptr                 m_root;
    MutexType                   m_mutex;
};

// 输出到控制台的Appender
class StdoutLogAppender : public LogAppender {
friend class Logger;
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log( Logger::ptr logger, LogLevel::Level level,
                LogEvent::ptr event ) override;

    std::string toYamlString() override;
private:
};

// 定义输出到文件的Appender
class FileLogAppender : public LogAppender {
friend class Logger;
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender( const std::string &filename );
    ~FileLogAppender() {m_filestream.close();}
    void log( Logger::ptr logger, LogLevel::Level level,
                LogEvent::ptr event ) override;
    // 重新打开文件，文件打开成功返回true
    bool reopen();
    std::string toYamlString() override;

private:
    std::string       m_filename;
    std::ofstream     m_filestream;
};

class LoggerManager {
public:
    using MutexType = Spinlock;
    LoggerManager();
    Logger::ptr getLogger(const std::string& name);

    void init();
    Logger::ptr getRoot() const { return m_root;}
    std::string toYamlString();
private:
    MutexType m_mutex;
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root;
};

typedef sylar::Singleton<sylar::LoggerManager> LoggerMgr;

} // namespace sylar

#endif