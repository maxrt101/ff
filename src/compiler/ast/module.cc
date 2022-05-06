#include <ff/ast/module.h>

ff::ast::Module::Module(const std::string& name, Node* body)
  : Node(NTYPE_MODULE), m_name(name), m_body(body) {}

std::string ff::ast::Module::getName() const {
  return m_name;
}

ff::ast::Node* ff::ast::Module::getBody() const {
  return m_body;
}
