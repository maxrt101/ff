#include <ff/ast/new.h>

ff::ast::New::New(Node* class_, bool isConstructorCalled, const std::vector<Node*>& constructorArgs)
  : Node(NTYPE_NEW), m_class(class_), m_isConstructorCalled(isConstructorCalled), m_constructorArgs(constructorArgs) {}

ff::ast::Node* ff::ast::New::getClass() const {
  return m_class;
}

bool ff::ast::New::getIsConstructorCalled() const {
  return m_isConstructorCalled;
}

std::vector<ff::ast::Node*> ff::ast::New::getConstructorArgs() const {
  return m_constructorArgs;
}
