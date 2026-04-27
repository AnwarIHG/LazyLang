#pragma once

#ifndef _LOGER_H
#define _LOGER_H

#include "./types.h"

#define LOG_WARN_ENABLE 1
#define LOG_INFO_ENABLE 1
#define LOG_DBUG_ENABLE 1
#define LOG_TRACE_ENABLE 1

#if RELEASE == 1
#define LOG_DBUG_ENABLE 0
#define LOG_TRACE_ENABLE 0
#endif

typedef enum {
  logSeverity_FATAL = 0,
  logSeverity_ERROR = 1,

  logSeverity_WARN = 2,
  logSeverity_INFO = 3,
  logSeverity_DBUG = 4,
  logSeverity_TRACE = 5,
} LogSeverity;

// TODO: id don't know if i realy need to implement this functions or not
// b8 log_initialize();
// void log_terminate();

void log_output(LogSeverity sev, const char *format, ...);

#if LOG_INFO_ENABLE == 1
#define INFO(message, ...)                                                     \
  log_output(logSeverity_INFO, message, ##__VA_ARGS__)
#else
#define INFO(message, ...)
#endif

#if LOG_WARN_ENABLE == 1
#define WARN(message, ...)                                                     \
  log_output(logSeverity_WARN, message, ##__VA_ARGS__)
#else
#define WARN(message, ...)
#endif

#ifndef FATAL
#define FATAL(message, ...)                                                    \
  log_output(logSeverity_FATAL, message, ##__VA_ARGS__)
#endif

#ifndef ERROR
#define ERROR(message, ...)                                                    \
  log_output(logSeverity_ERROR, message, ##__VA_ARGS__)
#endif

#if LOG_DBUG_ENABLE == 1
#define DBUG(message, ...)                                                     \
  log_output(logSeverity_DBUG, message, ##__VA_ARGS__)
#else
#define DBUG(message, ...)
#endif

#if LOG_TRACE_ENABLE == 1
#define TRACE(message, ...)                                                    \
  log_output(logSeverity_TRACE, message, ##__VA_ARGS__)
#else
#define TRACE(message, ...)
#endif

#endif //_LOGER_H
