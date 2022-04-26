#ifndef _FF_AST_FUNCTION_H_
#define _FF_AST_FUNCTION_H_ 1

#include <ff/ast/node.h>
#include <ff/ast/block.h>
#include <ff/ast/var_decl_list.h>
#include <ff/compiler/scanner.h>

namespace ff {
namespace ast {

class Function : public Node {
 private:
  Token m_name;
  VarDeclList* m_args;
  Ref<FunctionAnnotation> m_type;
  Node* m_body;

 public:
  Function(Token name, VarDeclList* args, Ref<FunctionAnnotation> type, Node* body);
  ~Function() = default;

  Token getName() const;
  VarDeclList* getArgs() const;
  Ref<FunctionAnnotation> getFunctionType() const;
  Node* getBody() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_FUNCTION_H_ */