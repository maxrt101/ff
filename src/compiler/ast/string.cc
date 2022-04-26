#include <ff/ast/string.h>

ff::ast::StringLiteral::StringLiteral(Token token) : Node(NTYPE_STRING_LITERAL), m_value(token) {}

std::string ff::ast::StringLiteral::getValue() const {
  return m_value.str;
}
