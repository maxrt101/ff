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

 public:
  New(Node* class_);
  ~New() = default;

  Node* getClass() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_NEW_H_ */