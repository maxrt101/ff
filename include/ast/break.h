#ifndef _FF_AST_BREAK_H_
#define _FF_AST_BREAK_H_ 1

#include <ff/ast/node.h>

namespace ff {
namespace ast {

class Break : public Node {
 private:
  Node* m_label;

 public:
  explicit Break(Node* label = nullptr);
  ~Break() = default;

  Node* getLabel() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_BREAK_H_ */