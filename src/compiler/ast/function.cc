#include <ff/ast/function.h>

ff::ast::Function::Function(const Token& name, VarDeclList* args, Ref<FunctionAnnotation> type, Node* body)
  : Node(NTYPE_FUNCTION), m_name(name), m_args(args), m_type(type), m_body(body) {}

ff::Token ff::ast::Function::getName() const {
  return m_name;
}

ff::ast::VarDeclList* ff::ast::Function::getArgs() const {
  return m_args;
}

ff::Ref<ff::FunctionAnnotation> ff::ast::Function::getFunctionType() const {
  return m_type;
}

ff::ast::Node* ff::ast::Function::getBody() const {
  return m_body;
}
