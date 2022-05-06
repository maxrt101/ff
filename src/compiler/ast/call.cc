#include <ff/ast/call.h>

ff::ast::Call::Call(Node* callee, const std::vector<Node*>& args, bool isReturnValueExpected)
  : Node(NTYPE_CALL), m_callee(callee), m_args(args), m_isReturnValueExpected(isReturnValueExpected) {}

ff::ast::Node* ff::ast::Call::getCallee() const {
  return m_callee;
}

std::vector<ff::ast::Node*> ff::ast::Call::getArgs() const {
  return m_args;
}

bool ff::ast::Call::isReturnValueExpected() const {
  return m_isReturnValueExpected;
}

void ff::ast::Call::setIsReturnValueExpected(bool value) {
  m_isReturnValueExpected = value;
}
