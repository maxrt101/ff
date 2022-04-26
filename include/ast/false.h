#ifndef _FF_AST_FALSE_H_
#define _FF_AST_FALSE_H_ 1

#include <ff/ast/node.h>
#include <string>

namespace ff {
namespace ast {

class False : public Node {
 public:
  False();
  ~False() = default;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_FALSE_H_ */