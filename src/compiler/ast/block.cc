#include <ff/ast/block.h>

ff::ast::Block::Block(const std::vector<Node*>& body)
  : Node(NTYPE_BLOCK), m_body(body) {}

std::vector<ff::ast::Node*> ff::ast::Block::getBody() const {
  return m_body;
}
