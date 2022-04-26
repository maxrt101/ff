#ifndef _FF_AST_CAST_H_
#define _FF_AST_CAST_H_ 1

#include <ff/ast/node.h>
#include <ff/compiler/type_annotation.h>
#include <vector>

namespace ff {
namespace ast {

class Cast : public Node {
 private:
  Node* m_value;
  // Token m_type;
  Ref<TypeAnnotation> m_type;

 public:
  Cast(Ref<TypeAnnotation> type, Node* value);
  ~Cast() = default;

  Node* getValue() const;
  Ref<TypeAnnotation> getCastType() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_CAST_H_ */