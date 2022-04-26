#ifndef _FF_AST_ASSIGNMENT_H_
#define _FF_AST_ASSIGNMENT_H_ 1

#include <ff/ast/node.h>
#include <vector>

namespace ff {
namespace ast {

class Assignment : public Node {
 private:
  Node* m_assignee;
  Node* m_value;

 public:
  Assignment(Node* assignee, Node* value);
  ~Assignment() = default;

  Node* getAssignee() const;
  Node* getValue() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_ASSIGNMENT_H_ */