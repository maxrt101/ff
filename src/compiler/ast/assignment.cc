#include <ff/ast/assignment.h>

ff::ast::Assignment::Assignment(Node* assignee, Node* value)
  : Node(NTYPE_ASSIGNMENT), m_assignee(assignee), m_value(value) {}

ff::ast::Node* ff::ast::Assignment::getAssignee() const {
  return m_assignee;
}

ff::ast::Node* ff::ast::Assignment::getValue() const {
  return m_value;
}
