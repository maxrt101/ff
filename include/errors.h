#ifndef _FF_ERRORS_H_
#define _FF_ERRORS_H_ 1

#include <ff/compiler/token.h>
#include <exception>
#include <string>

namespace ff {

class ScanError : public std::exception {
 private:
  int m_line;
  std::string m_filename;
  std::string m_message;

 public:
  ScanError(const std::string& filename, int line, const std::string& msg);
  ScanError(const std::string& filename, int line, const char* fmt, ...);

  const char* what() const noexcept override;

  void print() const;
};

class ParseError : public std::exception {
 private:
  Token m_token;
  std::string m_filename;
  std::string m_message;

 public:
  ParseError(Token token, const std::string& filename, const std::string& msg);

  const char* what() const noexcept override;

  void print() const;
};

class CompileError : public std::exception {
 private:
  int m_line;
  std::string m_filename;
  std::string m_message;
  std::string m_note;

 public:
  CompileError(const std::string& filename, int line, const std::string& msg);
  CompileError(const std::string& filename, int line, const char* fmt, ...);

  CompileError& addNote(const std::string& msg);
  CompileError& addNote(const char* fmt, ...);

  const char* what() const noexcept override;

  void print() const;
};

class RuntimeError : public std::exception {
 private:
  int m_line;
  std::string m_filename;
  std::string m_message;

 public:
  // RuntimeError(const std::string& msg);
  // RuntimeError(const char* fmt, ...);
  RuntimeError(const std::string& filename, int line, const std::string& msg);
  // RuntimeError(const std::string& filename, int line, const char* fmt, ...);
  // RuntimeError(const std::string& filename, int line, const char* fmt, va_list args);

  static RuntimeError flcreate(const std::string& filename, int line, const std::string& msg);
  static RuntimeError flcreatef(const std::string& filename, int line, const char* fmt, ...);
  static RuntimeError flvcreatef(const std::string& filename, int line, const char* fmt, va_list args);
  static RuntimeError create(const std::string& msg);
  static RuntimeError createf(const char* fmt, ...);
  static RuntimeError vcreatef(const char* fmt, va_list args);

  const char* what() const noexcept override;

  void print() const;
};

} /* namespace ff */

#endif /* FF_ERRORS_H_ */