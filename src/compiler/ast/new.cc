#include <ff/ast/new.h>

ff::ast::New::New(Node* class_)
  : Node(NTYPE_NEW), m_class(class_) {}

ff::ast::Node* ff::ast::New::getClass() const {
  return m_class;
}
