#ifndef _FF_AST_BREAKPOINT_H_
#define _FF_AST_BREAKPOINT_H_ 1

#include <ff/ast/node.h>

namespace ff {
namespace ast {

class Breakpoint : public Node {
 public:
  Breakpoint();
  ~Breakpoint() = default;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_BREAKPOINT_H_ */