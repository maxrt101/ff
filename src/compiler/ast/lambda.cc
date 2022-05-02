#include <ff/ast/lambda.h>

ff::ast::Lambda::Lambda(VarDeclList* args, Ref<FunctionAnnotation> type, Node* body)
  : Node(NTYPE_LAMBDA), m_args(args), m_type(type), m_body(body) {}


ff::ast::VarDeclList* ff::ast::Lambda::getArgs() const {
  return m_args;
}

ff::Ref<ff::FunctionAnnotation> ff::ast::Lambda::getFunctionType() const {
  return m_type;
}

ff::ast::Node* ff::ast::Lambda::getBody() const {
  return m_body;
}
