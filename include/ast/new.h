#ifndef _FF_AST_NEW_H_
#define _FF_AST_NEW_H_ 1

#include <ff/ast/node.h>
#include <ff/compiler/scanner.h>
#include <ff/compiler/type_annotation.h>
#include <unordered_map>
#include <vector>

namespace ff {
namespace ast {

class New : public Node {
 private:
  Node* m_class;
  bool m_isConstructorCalled = true;
  std::vector<Node*> m_constructorArgs;

 public:
  New(Node* class_, bool isConstructorCalled, const std::vector<Node*>& constructorArgs);
  ~New() = default;

  Node* getClass() const;
  bool getIsConstructorCalled() const;
  std::vector<Node*> getConstructorArgs() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_NEW_H_ */