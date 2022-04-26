#ifndef _FF_AST_FLOAT_H_
#define _FF_AST_FLOAT_H_ 1

#include <ff/ast/node.h>
#include <ff/compiler/scanner.h>

namespace ff {
namespace ast {

class FloatLiteral : public Node {
 private:
  Token m_value;

 public:
  FloatLiteral(Token token);
  ~FloatLiteral() = default;

  // void interpret() override;

  float getValue() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_FLOAT_H_ */