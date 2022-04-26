#ifndef _FF_AST_IDENTIFIER_H_
#define _FF_AST_IDENTIFIER_H_ 1

#include <ff/ast/node.h>
#include <ff/compiler/scanner.h>
#include <string>

namespace ff {
namespace ast {

class Identifier : public Node {
 private:
  Token m_value;

 public:
  Identifier(Token token);
  ~Identifier() = default;

  std::string getValue() const;

  std::string toString() const override;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_IDENTIFIER_H_ */