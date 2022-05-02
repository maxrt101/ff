#include <ff/ast/assignment.h>

ff::ast::Assignment::Assignment(Node* assignee, Node* value, bool isRefAssignment)
  : Node(NTYPE_ASSIGNMENT), m_assignee(assignee), m_value(value), m_isRefAssignment(isRefAssignment) {}

ff::ast::Node* ff::ast::Assignment::getAssignee() const {
  return m_assignee;
}

ff::ast::Node* ff::ast::Assignment::getValue() const {
  return m_value;
}

bool ff::ast::Assignment::getIsRefAssignment() const {
  return m_isRefAssignment;
}
