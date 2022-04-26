#ifndef _FF_AST_REF_H_
#define _FF_AST_REF_H_ 1

#include <ff/ast/node.h>
#include <ff/compiler/scanner.h>
#include <string>

namespace ff {
namespace ast {

class Ref : public Node {
 private:
  Node* m_value;

 public:
  Ref(Node* value);
  ~Ref() = default;

  Node* getValue() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_REF_H_ */