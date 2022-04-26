#include <ff/ast/ref.h>

ff::ast::Ref::Ref(Node* value) : Node(NTYPE_REF), m_value(value) {}

ff::ast::Node* ff::ast::Ref::getValue() const {
  return m_value;
}
