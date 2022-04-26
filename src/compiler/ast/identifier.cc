#include <ff/ast/identifier.h>

ff::ast::Identifier::Identifier(Token token) : Node(NTYPE_IDENTIFIER), m_value(token) {}

std::string ff::ast::Identifier::getValue() const {
  return m_value.str;
}

std::string ff::ast::Identifier::toString() const {
  return m_value.str;
}
