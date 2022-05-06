#ifndef _FF_LOG_H_
#define _FF_LOG_H_ 1

#include <string>

namespace ff {

enum class LogLevel {
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  FATAL
};

LogLevel stringToLogLevel(const std::string& s);

void vflogf(FILE* dest, LogLevel level, const std::string& format, va_list args);
void vlogf(LogLevel level, const std::string& format, va_list args);
void logf(LogLevel level, const std::string format, ...); // cppcheck-suppress passedByValue

void debug(const std::string format, ...); // cppcheck-suppress passedByValue
void info(const std::string format, ...); // cppcheck-suppress passedByValue
void warning(const std::string format, ...); // cppcheck-suppress passedByValue
void error(const std::string format, ...); // cppcheck-suppress passedByValue
void fatal(const std::string format, ...); // cppcheck-suppress passedByValue

} /* namespace ff */

#endif /* _FF_LOG_H_ */
