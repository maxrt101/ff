#include <ff/ast/foreach.h>

ff::ast::ForEach::ForEach(Node* loopVariable, Node* iterable, Node* body)
  : Node(NTYPE_FOREACH), m_loopVariable(loopVariable), m_iterable(iterable), m_body(body) {}

ff::ast::Node* ff::ast::ForEach::getLoopVariable() const {
  return m_loopVariable;
}

ff::ast::Node* ff::ast::ForEach::getIterable() const {
  return m_iterable;
}

ff::ast::Node* ff::ast::ForEach::getBody() const {
  return m_body;
}
