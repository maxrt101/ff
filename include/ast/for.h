#ifndef _FF_AST_FOR_H_
#define _FF_AST_FOR_H_ 1

#include <ff/ast/node.h>

namespace ff {
namespace ast {

class For : public Node {
 private:
  Node* m_init;
  Node* m_condition;
  Node* m_increment;
  Node* m_body;

 public:
  For(Node* init, Node* condition, Node* increment, Node* body);
  ~For() = default;

  Node* getInit() const;
  Node* getCondition() const;
  Node* getIncrement() const;
  Node* getBody() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_FOR_H_ */