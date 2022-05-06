#ifndef _FF_AST_VAR_DECL_H_
#define _FF_AST_VAR_DECL_H_ 1

#include <ff/ast/node.h>
#include <ff/compiler/scanner.h>
#include <ff/compiler/type_annotation.h>
#include <vector>

namespace ff {
namespace ast {

class VarDecl : public Node {
 private:
  Token m_name;
  Ref<TypeAnnotation> m_type;
  Node* m_value; // nullptr - no value
  bool m_const;

 public:
  VarDecl(const Token& name, Ref<TypeAnnotation> type, Node* value = nullptr, bool isConst = false);
  ~VarDecl() = default;

  Token getName() const;
  Ref<TypeAnnotation> getVarType() const;
  Node* getValue() const;
  bool getConst() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_VAR_DECL_H_ */