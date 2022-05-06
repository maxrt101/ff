#include <ff/ast/dict.h>

ff::ast::Dict::Dict(const std::map<std::string, Node*>& fields) : Node(NTYPE_DICT), m_fields(fields) {}

std::map<std::string, ff::ast::Node*> ff::ast::Dict::getFields() const {
  return m_fields;
}
