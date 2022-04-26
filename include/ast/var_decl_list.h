#ifndef _FF_AST_VAR_DECL_LIST_H_
#define _FF_AST_VAR_DECL_LIST_H_ 1

#include <ff/ast/node.h>
#include <ff/ast/var_decl.h>
#include <ff/compiler/scanner.h>
#include <vector>

namespace ff {
namespace ast {

class VarDeclList : public Node {
 private:
  std::vector<VarDecl*> m_list;

 public:
  VarDeclList(const std::vector<VarDecl*>& list);
  ~VarDeclList() = default;

  std::vector<VarDecl*> getList() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_VAR_DECL_LIST_H_ */