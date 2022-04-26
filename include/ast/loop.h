#ifndef _FF_AST_LOOP_H_
#define _FF_AST_LOOP_H_ 1

#include <ff/ast/node.h>

namespace ff {
namespace ast {

class Loop : public Node {
 private:
  Node* m_body;
  // name/label

 public:
  Loop(Node* body);
  ~Loop() = default;

  Node* getBody() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_LOOP_H_ */