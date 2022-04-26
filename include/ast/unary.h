#ifndef _FF_AST_UNARY_H_
#define _FF_AST_UNARY_H_ 1

#include <ff/ast/node.h>
#include <ff/compiler/scanner.h>
#include <string>

namespace ff {
namespace ast {

class Unary : public Node {
 private:
  Token m_op;
  Node* m_expr;

 public:
  Unary(Token op, Node* node);
  ~Unary() = default;

  Token getOperator() const;
  Node* getValue() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_UNARY_H_ */