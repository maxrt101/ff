#ifndef _FF_COMPILER_TOKEN_H_
#define _FF_COMPILER_TOKEN_H_ 1

#include <string>

namespace ff {

enum TokenType {
  TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
  TOKEN_COMMA, TOKEN_DOT,
  TOKEN_SEMICOLON, TOKEN_COLON,
  TOKEN_MINUS, TOKEN_PLUS, TOKEN_SLASH, TOKEN_STAR,
  TOKEN_DOLLAR, TOKEN_PERCENT, TOKEN_AMP, TOKEN_AT, TOKEN_VBAR,
  TOKEN_INCREMENT, TOKEN_DECREMENT,

  TOKEN_BANG, TOKEN_BANG_EQUAL,
  TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
  TOKEN_GREATER, TOKEN_GREATER_EQUAL,
  TOKEN_LESS, TOKEN_LESS_EQUAL,
  TOKEN_LEFT_ARROW, TOKEN_RIGHT_ARROW,

  TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

  TOKEN_CLASS, TOKEN_FN, TOKEN_VAR, TOKEN_CONST,
  TOKEN_NULL, TOKEN_TRUE, TOKEN_FALSE,
  TOKEN_IF, TOKEN_ELSE,
  TOKEN_FOR, TOKEN_WHILE, TOKEN_LOOP,
  TOKEN_BREAK, TOKEN_CONTINUE,
  TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
  TOKEN_AND, TOKEN_OR,
  TOKEN_AS, TOKEN_IN,
  TOKEN_NEW, TOKEN_REF,
  TOKEN_PRINT,
  TOKEN_BREAKPOINT,

  TOKEN_EOF,
  _TOKEN_ERROR
};

std::string tokenTypeToString(TokenType type);

struct Token {
  TokenType type;
  std::string str;
  int line;

  Token();
  Token(TokenType type, std::string str, int line);
  Token(TokenType type, const char* str, size_t length, int line);

  bool operator==(const Token& t) const;
  bool operator==(const std::string& s) const;

  int32_t toInteger() const;
  float toFloat() const;
};

} /* namespace ff */

#endif /* _FF_COMPILER_TOKEN_H_ */