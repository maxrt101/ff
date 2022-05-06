#ifndef _FF_AST_GROUP_H_
#define _FF_AST_GROUP_H_ 1

#include <ff/ast/node.h>
#include <ff/compiler/scanner.h>
#include <string>

namespace ff {
namespace ast {

class Group : public Node {
 private:
  Node* m_expr;

 public:
  explicit Group(Node* node);
  ~Group() = default;

  Node* getValue() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_GROUP_H_ */