#include <ff/ast/import.h>

ff::ast::Import::Import(const std::vector<std::string>& imports)
  : Node(NTYPE_IMPORT), m_imports(imports) {}

std::vector<std::string> ff::ast::Import::getImports() const {
  return m_imports;
}
