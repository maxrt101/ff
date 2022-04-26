#include <ff/ast/break.h>

ff::ast::Break::Break(Node* label) : Node(NTYPE_BREAK), m_label(label) {}

ff::ast::Node* ff::ast::Break::getLabel() const {
  return m_label;
}
