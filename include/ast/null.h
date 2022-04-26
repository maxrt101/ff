#ifndef _FF_AST_NULL_H_
#define _FF_AST_NULL_H_ 1

#include <ff/ast/node.h>
#include <string>

namespace ff {
namespace ast {

class Null : public Node {
 public:
  Null();
  ~Null() = default;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_NULL_H_ */