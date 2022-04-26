#ifndef _FF_COMPILER_SCANNER_H_
#define _FF_COMPILER_SCANNER_H_ 1

#include <ff/compiler/token.h>
#include <ff/errors.h>
#include <unordered_map>
#include <exception>
#include <vector>
#include <string>

namespace ff {

class Scanner {
 private:
  std::string m_filename;
  std::string m_source;

  const char* m_start = nullptr;
  const char* m_current = nullptr;

  bool m_hadError = false;
  int m_line = 1;
  int m_index = 0;

 public:
  Scanner(const std::string& filename, const std::string& source);

  std::vector<Token> tokenize();

 private:
  Token scanToken();
  void skipWhitespace();
  bool isAtEnd();
  char peek();
  char peekNext();
  bool match(char expected);
  char advance();
  char advance(int count);
  Token makeToken(TokenType type);
  TokenType identifierType();
  TokenType checkKeyword(int start, int length, const char* rest, TokenType type) const;

  Token identifier();
  Token character();
  Token number();
  Token string();
};

} /* namespace ff */

#endif /* _FF_COMPILER_SCANNER_H_ */