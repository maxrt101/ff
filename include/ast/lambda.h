#ifndef _FF_AST_LAMBDA_H_
#define _FF_AST_LAMBDA_H_ 1

#include <ff/ast/node.h>
#include <ff/ast/block.h>
#include <ff/ast/var_decl_list.h>
#include <ff/compiler/scanner.h>

namespace ff {
namespace ast {

class Lambda : public Node {
 private:
  VarDeclList* m_args;
  Ref<FunctionAnnotation> m_type;
  Node* m_body;

 public:
  Lambda(VarDeclList* args, Ref<FunctionAnnotation> type, Node* body);
  ~Lambda() = default;

  VarDeclList* getArgs() const;
  Ref<FunctionAnnotation> getFunctionType() const;
  Node* getBody() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_LAMBDA_H_ */