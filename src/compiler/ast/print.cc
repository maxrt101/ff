#include <ff/ast/print.h>

ff::ast::Print::Print(Node* node) : Node(NTYPE_PRINT), m_expr(node) {}

ff::ast::Node* ff::ast::Print::getValue() const {
  return m_expr;
}
