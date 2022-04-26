#include <ff/ast/if.h>

ff::ast::If::If(Node* condition, Node* body, Node* elseBody) : Node(NTYPE_IF), m_condition(condition), m_body(body), m_elseBody(elseBody) {}

ff::ast::Node* ff::ast::If::getCondition() const {
  return m_condition;
}

ff::ast::Node* ff::ast::If::getBody() const {
  return m_body;
}

ff::ast::Node* ff::ast::If::getElseBody() const {
  return m_elseBody;
}
