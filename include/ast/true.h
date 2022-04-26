#ifndef _FF_AST_TRUE_H_
#define _FF_AST_TRUE_H_ 1

#include <ff/ast/node.h>
#include <string>

namespace ff {
namespace ast {

class True : public Node {
 public:
  True();
  ~True() = default;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_TRUE_H_ */