#include <ff/ast/cast.h>

ff::ast::Cast::Cast(Ref<TypeAnnotation> type, Node* value)
  : Node(NTYPE_CAST_EXPR), m_type(type), m_value(value) {}

ff::ast::Node* ff::ast::Cast::getValue() const {
  return m_value;
}

ff::Ref<ff::TypeAnnotation> ff::ast::Cast::getCastType() const {
  return m_type;
}
