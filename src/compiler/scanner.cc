#include <ff/compiler/scanner.h>
#include <ff/utils/str.h>
#include <mrt/console/colors.h>
#include <unistd.h>
#include <cstring>
#include <cstdarg>
#include <map>

static std::map<ff::TokenType, std::string> g_tokenTypes {
  {ff::TOKEN_LEFT_PAREN, "TOKEN_LEFT_PAREN"},
  {ff::TOKEN_RIGHT_PAREN, "TOKEN_RIGHT_PAREN"},
  {ff::TOKEN_LEFT_BRACE, "TOKEN_LEFT_BRACE"},
  {ff::TOKEN_RIGHT_BRACE, "TOKEN_RIGHT_BRACE"},
  {ff::TOKEN_COMMA, "TOKEN_COMMA"},
  {ff::TOKEN_DOT, "TOKEN_DOT"},
  {ff::TOKEN_SEMICOLON, "TOKEN_SEMICOLON"},
  {ff::TOKEN_COLON, "TOKEN_COLON"},
  {ff::TOKEN_MINUS, "TOKEN_MINUS"},
  {ff::TOKEN_PLUS, "TOKEN_PLUS"},
  {ff::TOKEN_SLASH, "TOKEN_SLASH"},
  {ff::TOKEN_STAR, "TOKEN_STAR"},
  {ff::TOKEN_DOLLAR, "TOKEN_DOLLAR"},
  {ff::TOKEN_PERCENT, "TOKEN_PERCENT"},
  {ff::TOKEN_AMP, "TOKEN_AMP"},
  {ff::TOKEN_VBAR, "TOKEN_VBAR"},
  {ff::TOKEN_INCREMENT, "TOKEN_INCREMENT"},
  {ff::TOKEN_DECREMENT, "TOKEN_DECREMENT"},
  {ff::TOKEN_AT, "TOKEN_AT"},
  {ff::TOKEN_BANG, "TOKEN_BANG"},
  {ff::TOKEN_BANG_EQUAL, "TOKEN_BANG_EQUAL"},
  {ff::TOKEN_EQUAL, "TOKEN_EQUAL"},
  {ff::TOKEN_COLON_EQUAL, "TOKEN_COLON_EQUAL"},
  {ff::TOKEN_EQUAL_EQUAL, "TOKEN_EQUAL_EQUAL"},
  {ff::TOKEN_GREATER, "TOKEN_GREATER"},
  {ff::TOKEN_GREATER_EQUAL, "TOKEN_GREATER_EQUAL"},
  {ff::TOKEN_LESS, "TOKEN_LESS"},
  {ff::TOKEN_LESS_EQUAL, "TOKEN_LESS_EQUAL"},
  {ff::TOKEN_LEFT_ARROW, "TOKEN_LEFT_ARROW"},
  {ff::TOKEN_RIGHT_ARROW, "TOKEN_RIGHT_ARROW"},
  {ff::TOKEN_IDENTIFIER, "TOKEN_IDENTIFIER"},
  {ff::TOKEN_STRING, "TOKEN_STRING"},
  {ff::TOKEN_NUMBER, "TOKEN_NUMBER"},
  {ff::TOKEN_CLASS, "TOKEN_CLASS"},
  {ff::TOKEN_FN, "TOKEN_FN"},
  {ff::TOKEN_VAR, "TOKEN_VAR"},
  {ff::TOKEN_CONST, "TOKEN_CONST"},
  {ff::TOKEN_NULL, "TOKEN_NULL"},
  {ff::TOKEN_TRUE, "TOKEN_TRUE"},
  {ff::TOKEN_FALSE, "TOKEN_FALSE"},
  {ff::TOKEN_IF, "TOKEN_IF"},
  {ff::TOKEN_ELSE, "TOKEN_ELSE"},
  {ff::TOKEN_FOR, "TOKEN_FOR"},
  {ff::TOKEN_WHILE, "TOKEN_WHILE"},
  {ff::TOKEN_LOOP, "TOKEN_LOOP"},
  {ff::TOKEN_PRINT, "TOKEN_PRINT"},
  {ff::TOKEN_RETURN, "TOKEN_RETURN"},
  {ff::TOKEN_SUPER, "TOKEN_SUPER"},
  {ff::TOKEN_THIS, "TOKEN_THIS"},
  {ff::TOKEN_AND, "TOKEN_AND"},
  {ff::TOKEN_OR, "TOKEN_OR"},
  {ff::TOKEN_BREAK, "TOKEN_BREAK"},
  {ff::TOKEN_CONTINUE, "TOKEN_CONTINUE"},
  {ff::TOKEN_AS, "TOKEN_AS"},
  {ff::TOKEN_IN, "TOKEN_IN"},
  {ff::TOKEN_NEW, "TOKEN_NEW"},
  {ff::TOKEN_REF, "TOKEN_REF"},
  {ff::TOKEN_EOF, "TOKEN_EOF"},
};

std::string ff::tokenTypeToString(ff::TokenType type) {
  if (g_tokenTypes.find(type) == g_tokenTypes.end()) {
    return "<unknown>";
  }
  return g_tokenTypes[type];
}

ff::ScanError::ScanError(const std::string& filename, int line, const std::string& msg) : m_line(line), m_filename(filename), m_message(msg) {}

ff::ScanError::ScanError(const std::string& filename, int line, const char* fmt, ...) : m_line(line), m_filename(filename) {
  char* buffer = new char[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, 1024, fmt, args);
  va_end(args);
  m_message = buffer;
  delete [] buffer;
}

const char* ff::ScanError::what() const noexcept {
  return m_message.c_str();
}

void ff::ScanError::print() const {
  printf("%s:%d: %sScanError%s: %s\n", m_filename.c_str(), m_line, mrt::console::RED, mrt::console::RESET, m_message.c_str());
}

ff::Token::Token() {}

ff::Token::Token(TokenType type, const std::string& str, int line) : type(type), str(str), line(line) {}

ff::Token::Token(TokenType type, const char* str, size_t length, int line) : type(type), line(line) {
  this->str = std::string(str, length);
}

bool ff::Token::operator==(const Token& t) const {
  return str == t.str;
}

bool ff::Token::operator==(const std::string& s) const {
  return str == s;
}

int32_t ff::Token::toInteger() const {
  return str::toInt(str);
}

float ff::Token::toFloat() const {
  return std::stof(str);
}

ff::Scanner::Scanner(const std::string& filename, const std::string& source) : m_filename(filename), m_source(source) {
  m_current = m_source.c_str();
  m_start = m_source.c_str();
  m_line = 1;
}

std::vector<ff::Token> ff::Scanner::tokenize() {
  std::vector<Token> result;
  Token token;
  do {
    token = scanToken();
    result.push_back(token);
  } while (token.type != TOKEN_EOF);
  return result;
}

ff::Token ff::Scanner::scanToken() {
  skipWhitespace();

  m_start = m_current;

  if (isAtEnd()) return makeToken(TOKEN_EOF);

  char c = advance();

  if (isalpha(c) || c == '_') return identifier();
  if (isdigit(c)) return number();

  switch (c) {
    case '(': return makeToken(TOKEN_LEFT_PAREN);
    case ')': return makeToken(TOKEN_RIGHT_PAREN);
    case '{': return makeToken(TOKEN_LEFT_BRACE);
    case '}': return makeToken(TOKEN_RIGHT_BRACE);
    case ';': return makeToken(TOKEN_SEMICOLON);
    case ':': return makeToken(match('=') ? TOKEN_COLON_EQUAL : TOKEN_COLON);
    case ',': return makeToken(TOKEN_COMMA);
    case '.': return makeToken(TOKEN_DOT);
    case '-': return makeToken(match('>') ? TOKEN_RIGHT_ARROW : (match('-') ? TOKEN_DECREMENT : TOKEN_MINUS));
    case '+': return makeToken(match('+') ? TOKEN_INCREMENT : TOKEN_PLUS);
    case '/': return makeToken(TOKEN_SLASH);
    case '*': return makeToken(TOKEN_STAR);
    case '%': return makeToken(TOKEN_PERCENT);
    case '$': return makeToken(TOKEN_DOLLAR);
    case '@': return makeToken(TOKEN_AT);
    case '&': return makeToken(match('&') ? TOKEN_AND : TOKEN_AMP);
    case '|': return makeToken(match('|') ? TOKEN_OR : TOKEN_VBAR);
    case '!': return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=': return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<': {
      if (match('-')) {
        return makeToken(TOKEN_LEFT_ARROW);
      }
      if (match('=')) {
        return makeToken(TOKEN_LESS_EQUAL);
      }
      return makeToken(TOKEN_LESS);
    }
    case '>': return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    case '"': return string();
  }

  throw ScanError(m_filename, m_line, "Unexpected char '%c'", c);
}

void ff::Scanner::skipWhitespace() {
  for (;;) {
    char c = peek();
    switch (c) {
      case '\n':
        m_line++; [[fallthrough]];
      case ' ':   [[fallthrough]];
      case '\r':  [[fallthrough]];
      case '\t':
        advance();
        break;
      case '/':
        if (peekNext() == '/') {
          while (peek() != '\n' && !isAtEnd()) advance();
        } else if (peekNext() == '*') {
          advance(2);
          while (!isAtEnd() && peek() != '*' && peekNext() != '/') {
            if (peek() == '\n') m_line++;
            advance();
          }
          advance(2);
        } else {
          return;
        }
        break;
      default:
        return;
    }
  }
}

bool ff::Scanner::isAtEnd() {
  return *m_current == '\0' || *m_current == EOF;
}

char ff::Scanner::peek() {
  if (isAtEnd()) return '\0';
  return *m_current;
}

char ff::Scanner::peekNext() {
  return m_current[1];
}

bool ff::Scanner::match(char expected) {
  if (isAtEnd()) return false;
  if (peek() != expected) return false;
  m_current++;
  return true;
}

char ff::Scanner::advance() {
  m_current++;
  return m_current[-1];
}

char ff::Scanner::advance(int count) {
  m_current += count;
  return m_current[-1];
}

ff::Token ff::Scanner::makeToken(TokenType type) {
  return Token(type, std::string(m_start, m_current), m_line);
}

ff::TokenType ff::Scanner::identifierType() {
  switch (m_start[0]) {
    case 'a':
      if (m_current - m_start > 1) {
        switch (m_start[1]) {
          case 's': return checkKeyword(1, 1, "s", TOKEN_AS);
        }
      }
      break;
    case 'b': {
      if (m_current - m_start > 1) {
        switch (m_start[1]) {
          case 'r': {
            if (m_current - m_start > 2) {
              switch (m_start[2]) {
                case 'e': {
                  if (m_current - m_start > 3) {
                    switch (m_start[3]) {
                      case 'a': {
                        if (m_current - m_start > 4) {
                          switch (m_start[4]) {
                            case 'k': {
                              if (m_current - m_start > 5) {
                                return checkKeyword(5, 5, "point", TOKEN_BREAKPOINT);
                              } else {
                                return checkKeyword(4, 1, "k", TOKEN_BREAK);
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      break;
    }
    case 'c': {
      if (m_current - m_start > 1) {
        switch (m_start[1]) {
          case 'l': return checkKeyword(2, 3, "ass", TOKEN_CLASS);
          case 'o': {
            if (m_current - m_start > 2) {
              switch (m_start[2]) {
                case 'n': {
                  if (m_current - m_start > 3) {
                    switch (m_start[3]) {
                      case 's': return checkKeyword(4, 1, "t", TOKEN_CONST);
                      case 't': return checkKeyword(4, 4, "inue", TOKEN_CONTINUE);
                    }
                  }
                }
              }
            }
          }
        }
      }
      break;
    }
    case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
    case 'f': {
      if (m_current - m_start > 1) {
        switch (m_start[1]) {
          case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
          case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
          case 'n': return checkKeyword(1, 1, "n", TOKEN_FN);
        }
      }
      break;
    }
    case 'i': {
      if (m_current - m_start > 1) {
        switch (m_start[1]) {
          case 'f': return checkKeyword(1, 1, "f", TOKEN_IF);
          case 'm': return checkKeyword(2, 4, "port", TOKEN_IMPORT);
          case 'n': return checkKeyword(1, 1, "n", TOKEN_IN);
        }
      }
      break;
    }
    case 'l': return checkKeyword(1, 3, "oop", TOKEN_LOOP);
    case 'm': return checkKeyword(1, 5, "odule", TOKEN_MODULE);
    case 'n': {
      if (m_current - m_start > 1) {
        switch (m_start[1]) {
          case 'e': return checkKeyword(2, 1, "w", TOKEN_NEW);
          case 'u': return checkKeyword(2, 2, "ll", TOKEN_NULL);
        }
      }
      break;
    }
    case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
    case 'r': {
      if (m_current - m_start > 1) {
        switch (m_start[1]) {
          case 'e': {
            if (m_current - m_start > 2) {
              switch (m_start[2]) {
                case 'f': return checkKeyword(2, 1, "f", TOKEN_REF);
                case 't': return checkKeyword(3, 3, "urn", TOKEN_RETURN);
              }
            }
          }
        }
      }
      break;
    }
    case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
    case 't': {
      if (m_current - m_start > 1) {
        switch (m_start[1]) {
          case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
          case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
        }
      }
      break;
    }
    case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
    case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
  }
  return TOKEN_IDENTIFIER;
}

ff::TokenType ff::Scanner::checkKeyword(int start, int length, const char* rest, TokenType type) const {
  if (m_current - m_start == start + length && !memcmp(m_start+start, rest, length) /*itrEqualsCharBuffer(m_start+start, rest, length)*/) {
    return type;
   }
   return TOKEN_IDENTIFIER;
}

ff::Token ff::Scanner::identifier() {
  while (isalnum(*m_current) || *m_current == '_') m_current++;
  return makeToken(identifierType());
}

ff::Token ff::Scanner::number() {
  while (isdigit(peek()) || peek() == 'x' || (peek() >= 'a' && peek() <= 'f') || peek() == '.') advance();
  std::string str(m_start, m_current - m_start);
  if (str.find('x') != std::string::npos || str.find('b') != std::string::npos) {
    if (str.find('.') != std::string::npos) {
      throw ScanError(m_filename, m_line, "Invalid number literal: '%s'", str.c_str());
    }
    if (str[0] != '0') {
      throw ScanError(m_filename, m_line, "Invalid number literal: '%s'", str.c_str());
    } else {
      if (str.size() > 3 && (str.substr(3).find('x') != std::string::npos || str.substr(3).find('b') != std::string::npos)) {
        throw ScanError(m_filename, m_line, "Invalid number literal: '%s'", str.c_str());
      }
    }
  }
  return Token(TOKEN_NUMBER, str, m_line);
}

ff::Token ff::Scanner::string() {
  if (peek() == '"') advance();
  m_start = m_current;
  std::string result;
  while (!isAtEnd()) {
    if (peek() == '\\') {
      advance();
      switch (peek()) {
        case '"':  result.push_back('"'); break;
        case '\'': result.push_back('\''); break;
        case 'n':  result.push_back('\n'); break;
        case 't':  result.push_back('\t'); break;
        case 'r':  result.push_back('\r'); break;
        default:   result.push_back(peek()); break;
      }
      advance();
      continue;
    }
    if (peek() == '"') {
      advance();
      break;
    }
    result.push_back(peek());
    advance();
  }
  return Token(TOKEN_STRING, result, m_line);
}
