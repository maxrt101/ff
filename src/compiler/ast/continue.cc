#include <ff/ast/continue.h>

ff::ast::Continue::Continue(Node* label) : Node(NTYPE_CONTINUE), m_label(label) {}

ff::ast::Node* ff::ast::Continue::getLabel() const {
  return m_label;
}
