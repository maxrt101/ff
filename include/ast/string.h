#ifndef _FF_AST_STRING_H_
#define _FF_AST_STRING_H_ 1

#include <ff/ast/node.h>
#include <ff/compiler/scanner.h>
#include <string>

namespace ff {
namespace ast {

class StringLiteral : public Node {
 private:
  Token m_value;

 public:
  StringLiteral(Token token);
  ~StringLiteral() = default;

  std::string getValue() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_STRING_H_ */