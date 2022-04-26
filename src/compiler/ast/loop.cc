#include <ff/ast/loop.h>

ff::ast::Loop::Loop(Node* body) : Node(NTYPE_LOOP), m_body(body) {}

ff::ast::Node* ff::ast::Loop::getBody() const {
  return m_body;
}
