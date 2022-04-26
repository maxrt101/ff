#include <ff/ast/group.h>

ff::ast::Group::Group(Node* node) : Node(NTYPE_GROUP_EXPR), m_expr(node) {}

ff::ast::Node* ff::ast::Group::getValue() const {
  return m_expr;
}
