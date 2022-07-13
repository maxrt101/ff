#include <ff/ast/class.h>

ff::ast::Class::Field::Field(
  Token name,
  Ref<TypeAnnotation> type,
  Node* value,
  bool isConst,
  bool isStatic
) : name(name), type(type), value(value), isConst(isConst), isStatic(isStatic) {}

ff::ast::Class::Method::Method(
  ff::ast::Function* fn,
  bool isStatic
) : fn(fn), isStatic(isStatic) {}

ff::ast::Class::Class(Token name, std::vector<Field>& fields, std::vector<ff::ast::Class::Method>& methods)
  : Node(NTYPE_CLASS), m_name(name), m_fields(fields), m_methods(methods) {}

ff::Token ff::ast::Class::getName() const {
  return m_name;
}

std::vector<ff::ast::Class::Field>& ff::ast::Class::getFields() {
  return m_fields;
}

std::vector<ff::ast::Class::Method>& ff::ast::Class::getMethods() {
  return m_methods;
}
