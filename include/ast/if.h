#ifndef _FF_AST_IF_H_
#define _FF_AST_IF_H_ 1

#include <ff/ast/node.h>

namespace ff {
namespace ast {

class If : public Node {
 private:
  Node* m_condition = nullptr;
  Node* m_body = nullptr;
  Node* m_elseBody = nullptr;

 public:
  If(Node* condition, Node* body, Node* elseBody);
  ~If() = default;

  Node* getCondition() const;
  Node* getBody() const;
  Node* getElseBody() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_IF_H_ */