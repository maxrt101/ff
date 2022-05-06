#ifndef _FF_AST_BINARY_H_
#define _FF_AST_BINARY_H_ 1

#include <ff/ast/node.h>
#include <ff/compiler/scanner.h>
#include <string>

namespace ff {
namespace ast {

class Binary : public Node {
 private:
  Token m_op;
  Node* m_left;
  Node* m_right;

 public:
  Binary(const Token& op, Node* left, Node* right);
  ~Binary() = default;

  Token getOperator() const;
  Node* getLeft() const;
  Node* getRight() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_BINARY_H_ */