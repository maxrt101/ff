#ifndef _FF_AST_PRINT_H_
#define _FF_AST_PRINT_H_ 1

#include <ff/ast/node.h>
#include <ff/compiler/scanner.h>
#include <string>

namespace ff {
namespace ast {

class Print : public Node {
 private:
  Node* m_expr;

 public:
  Print(Node* node);
  ~Print() = default;

  Node* getValue() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_PRINT_H_ */