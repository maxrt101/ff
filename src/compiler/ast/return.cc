#include <ff/ast/return.h>

ff::ast::Return::Return(Node* expr)
  : Node(NTYPE_RETURN), m_expr(expr) {}

ff::ast::Node* ff::ast::Return::getValue() const {
  return m_expr;
}
