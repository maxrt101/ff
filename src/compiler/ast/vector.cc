#include <ff/ast/vector.h>

ff::ast::Vector::Vector(std::vector<Node*> elements) : Node(NTYPE_VECTOR), m_elements(elements) {}

std::vector<ff::ast::Node*> ff::ast::Vector::getElements() const {
  return m_elements;
}
