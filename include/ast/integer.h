#ifndef _FF_AST_INTEGER_H_
#define _FF_AST_INTEGER_H_ 1

#include <ff/ast/node.h>
#include <ff/compiler/scanner.h>
#include <cstdint>

namespace ff {
namespace ast {

class IntegerLiteral : public Node {
 private:
  Token m_value;

 public:
  explicit IntegerLiteral(const Token& token);
  ~IntegerLiteral() = default;

  int32_t getValue() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_INTEGER_H_ */