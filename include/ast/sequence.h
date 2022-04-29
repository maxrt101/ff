#ifndef _FF_AST_SEQUENCE_H_
#define _FF_AST_SEQUENCE_H_ 1

#include <ff/ast/node.h>
#include <ff/ast/identifier.h>
#include <vector>

namespace ff {
namespace ast {

class Sequence : public Node {
 private:
  std::vector<Node*> m_sequence;

 public:
  Sequence(const std::vector<Node*>& sequence);
  ~Sequence() = default;

  std::vector<Node*> getSequence() const;
  bool hasFunctionCalls() const;

  std::string toString() const override;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_SEQUENCE_H_ */