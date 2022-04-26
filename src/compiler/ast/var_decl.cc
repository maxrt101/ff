#include <ff/ast/var_decl.h>

ff::ast::VarDecl::VarDecl(Token name, Ref<TypeAnnotation> type, Node* value, bool isConst)
  : Node(NTYPE_VAR_DECL), m_name(name), m_type(type), m_value(value), m_const(isConst) {}

ff::Token ff::ast::VarDecl::getName() const {
  return m_name;
}

ff::Ref<ff::TypeAnnotation> ff::ast::VarDecl::getVarType() const {
  return m_type;
}

ff::ast::Node* ff::ast::VarDecl::getValue() const {
  return m_value;
}

bool ff::ast::VarDecl::getConst() const {
  return m_const;
}
