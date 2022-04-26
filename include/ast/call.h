#ifndef _FF_AST_CALL_H_
#define _FF_AST_CALL_H_ 1

#include <ff/ast/node.h>
#include <vector>

namespace ff {
namespace ast {

class Call : public Node {
 private:
  Node* m_callee;
  std::vector<Node*> m_args;
  bool m_isReturnValueExpected;

 public:
  Call(Node* callee, std::vector<Node*> args, bool isReturnValueExpected);
  ~Call() = default;

  Node* getCallee() const;
  std::vector<Node*> getArgs() const;
  bool isReturnValueExpected() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_CALL_H_ */