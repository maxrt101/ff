#include <ff/ast/unary.h>

ff::ast::Unary::Unary(Token op, Node* node) : Node(NTYPE_UNARY_EXPR), m_op(op), m_expr(node) {}

ff::Token ff::ast::Unary::getOperator() const {
  return m_op;
}

ff::ast::Node* ff::ast::Unary::getValue() const {
  return m_expr;
}
