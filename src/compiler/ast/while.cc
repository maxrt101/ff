#include <ff/ast/while.h>

ff::ast::While::While(Node* condition, Node* body) : Node(NTYPE_WHILE), m_condition(condition), m_body(body) {}

ff::ast::Node* ff::ast::While::getCondition() const {
  return m_condition;
}

ff::ast::Node* ff::ast::While::getBody() const {
  return m_body;
}
