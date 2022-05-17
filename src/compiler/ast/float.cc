#include <ff/ast/float.h>
#include <cstdio>

ff::ast::FloatLiteral::FloatLiteral(const Token& token) : Node(NTYPE_FLOAT_LITERAL), m_value(token) {}

float ff::ast::FloatLiteral::getValue() const {
  return m_value.toFloat();
}