#ifndef _FF_AST_CONTINUE_H_
#define _FF_AST_CONTINUE_H_ 1

#include <ff/ast/node.h>

namespace ff {
namespace ast {

class Continue : public Node {
 private:
  Node* m_label;

 public:
  explicit Continue(Node* label = nullptr);
  ~Continue() = default;

  Node* getLabel() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_CONTINUE_H_ */