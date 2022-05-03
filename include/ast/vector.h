#ifndef _FF_AST_VECTOR_H_
#define _FF_AST_VECTOR_H_ 1

#include <ff/ast/node.h>
#include <vector>

namespace ff {
namespace ast {

class Vector : public Node {
 private:
  std::vector<Node*> m_elements;

 public:
  Vector(std::vector<Node*> elements);
  ~Vector() = default;

  std::vector<Node*> getElements() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_VECTOR_H_ */