#ifndef _FF_AST_WHILE_H_
#define _FF_AST_WHILE_H_ 1

#include <ff/ast/node.h>

namespace ff {
namespace ast {

class While : public Node {
 private:
  Node* m_condition;
  Node* m_body;

 public:
  While(Node* condition, Node* body);
  ~While() = default;

  Node* getCondition() const;
  Node* getBody() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_WHILE_H_ */