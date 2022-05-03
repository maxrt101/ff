#include <ff/log.h>
#include <mrt/console/colors.h>
#include <algorithm>

#define _VLOGF_INTERNAL(level) \
  va_list args; \
  va_start(args, format); \
  vlogf(level, format, args); \
  va_end(args);

ff::LogLevel ff::stringToLogLevel(const std::string& s) {
  std::string str = s;
  std::transform(str.begin(), str.end(), str.begin(), tolower);
  if (str == "debug") {
    return LogLevel::DEBUG;
  } else if (str == "info") {
    return LogLevel::INFO;
  } else if (str == "warning") {
    return LogLevel::WARNING;
  } else if (str == "error") {
    return LogLevel::ERROR;
  } else if (str == "fatal") {
    return LogLevel::FATAL;
  }
  return LogLevel::DEBUG;
}

void ff::vflogf(FILE* dest, LogLevel level, const std::string& format, va_list args) {
  switch (level) {
    case LogLevel::FATAL:   fprintf(dest, "%sFATAL",   mrt::console::RED_RED); break;
    case LogLevel::ERROR:   fprintf(dest, "%sERROR",   mrt::console::RED);     break;
    case LogLevel::WARNING: fprintf(dest, "%sWARNING", mrt::console::YELLOW);  break;
    case LogLevel::INFO:    fprintf(dest, "%sINFO",    mrt::console::CYAN);    break;
    case LogLevel::DEBUG:   fprintf(dest, "%sDEBUG",   mrt::console::BLUE);    break;
    default:                fprintf(dest, "INVALID_LOG_LEVEL");          break;
  }

  fprintf(dest, "\033[0m: ");
  vfprintf(dest, format.c_str(), args);
  fprintf(dest, "\n");
}

void ff::vlogf(LogLevel level, const std::string& format, va_list args) {
  FILE* dest = stdout;
#ifdef _FF_LOG_STDOUT_ONLY
  if (level > LogLevel::INFO) {
    dest = stderr;
  }
#endif

  vflogf(dest, level, format, args);
}

void ff::logf(LogLevel level, const std::string format, ...) {
  va_list args;
  va_start(args, format);
  vlogf(level, format, args);
  va_end(args);
}

void ff::debug(const std::string format, ...) {
  _VLOGF_INTERNAL(LogLevel::DEBUG);
}

void ff::info(const std::string format, ...) {
  _VLOGF_INTERNAL(LogLevel::INFO);
}

void ff::warning(const std::string format, ...) {
  _VLOGF_INTERNAL(LogLevel::WARNING);
}

void ff::error(const std::string format, ...) {
  _VLOGF_INTERNAL(LogLevel::ERROR);
}

void ff::fatal(const std::string format, ...) {
  _VLOGF_INTERNAL(LogLevel::FATAL);
}
