#include <ff/ast/for.h>

ff::ast::For::For(Node* init, Node* condition, Node* increment, Node* body) : Node(NTYPE_FOR), m_init(init), m_condition(condition), m_increment(increment), m_body(body) {}

ff::ast::Node* ff::ast::For::getInit() const {
  return m_init;
}

ff::ast::Node* ff::ast::For::getCondition() const {
  return m_condition;
}

ff::ast::Node* ff::ast::For::getIncrement() const {
  return m_increment;
}

ff::ast::Node* ff::ast::For::getBody() const {
  return m_body;
}
