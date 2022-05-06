#include <ff/ast/var_decl_list.h>

ff::ast::VarDeclList::VarDeclList(const std::vector<VarDecl*>& list)
  : Node(NTYPE_VAR_DECL_LIST), m_list(list) {}

std::vector<ff::ast::VarDecl*>& ff::ast::VarDeclList::getList() {
  return m_list;
}
