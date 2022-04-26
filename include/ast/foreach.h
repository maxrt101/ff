#ifndef _FF_AST_FOREACH_H_
#define _FF_AST_FOREACH_H_ 1

#include <ff/ast/node.h>

namespace ff {
namespace ast {

class ForEach : public Node {
 private:
  Node* m_loopVariable;
  Node* m_iterable;
  Node* m_body;

 public:
  ForEach(Node* loopVariable, Node* iterable, Node* body);
  ~ForEach() = default;

  Node* getLoopVariable() const;
  Node* getIterable() const;
  Node* getBody() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_FOREACH_H_ */