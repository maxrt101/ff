#ifndef _FF_AST_BLOCK_H_
#define _FF_AST_BLOCK_H_ 1

#include <ff/ast/node.h>
#include <vector>

namespace ff {
namespace ast {

class Block : public Node {
 private:
  std::vector<Node*> m_body;

 public:
  Block(const std::vector<Node*>& body);
  ~Block() = default;

  std::vector<Node*> getBody() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_BLOCK_H_ */