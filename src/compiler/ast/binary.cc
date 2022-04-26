#include <ff/ast/binary.h>

ff::ast::Binary::Binary(Token op, Node* left, Node* right)
  : Node(NTYPE_BINARY_EXPR), m_op(op), m_left(left), m_right(right) {}

ff::Token ff::ast::Binary::getOperator() const {
  return m_op;
}

ff::ast::Node* ff::ast::Binary::getLeft() const {
  return m_left;
}

ff::ast::Node* ff::ast::Binary::getRight() const {
  return m_right;
}
