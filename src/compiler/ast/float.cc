#include <ff/ast/float.h>
#include <cstdio>

ff::ast::FloatLiteral::FloatLiteral(const Token& token) : Node(NTYPE_FLOAT_LITERAL), m_value(token) {}

float ff::ast::FloatLiteral::getValue() const {
  printf("FloatLiteral::getValue() value='%s'\n", m_value.str.c_str());
  return m_value.toFloat();
}