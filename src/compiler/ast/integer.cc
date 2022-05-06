#include <ff/ast/integer.h>

ff::ast::IntegerLiteral::IntegerLiteral(const Token& token) : Node(NTYPE_INTEGER_LITERAL), m_value(token) {}

int32_t ff::ast::IntegerLiteral::getValue() const {
  return m_value.toInteger();
}
