#ifndef _FF_AST_RETURN_H_
#define _FF_AST_RETURN_H_ 1

#include <ff/ast/node.h>
#include <ff/compiler/scanner.h>
#include <string>

namespace ff {
namespace ast {

class Return : public Node {
 private:
  Node* m_expr;

 public:
  Return(Node* expr);
  ~Return() = default;

  Node* getValue() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_RETURN_H_ */