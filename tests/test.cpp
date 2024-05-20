#include <iostream>
#include <thread>

#include "sylar/util.h"
#include "sylar/log.h"

int main(int ragc, char** argv){
    sylar::Logger::ptr logger(new sylar::Logger());
    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));
    
    sylar::LogFormatter::ptr fmt(new sylar::LogFormatter("%d%T[%p]%T%m%n"));
    sylar::FileLogAppender::ptr file_appender(new sylar::FileLogAppender("./log.txt"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(sylar::LogLevel::Level::ERROR);

    logger->addAppender(file_appender);

    SYLAR_LOG_INFO(logger) << "test SYLAR_LOG_INFO";
    SYLAR_LOG_ERROR(logger) << "test SYLAR_LOG_ERROR";
    SYLAR_LOG_FMT_INFO(logger, "SYLAR_LOG_FMT_INFO %s", "ss");

    auto l = sylar::LoggerMgr::GetIstance()->getLogger("xx");
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "xxx";
    
    return 0;
}

