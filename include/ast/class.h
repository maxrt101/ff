#ifndef _FF_AST_CLASS_H_
#define _FF_AST_CLASS_H_ 1

#include <ff/ref.h>
#include <ff/ast/node.h>
#include <ff/ast/function.h>
#include <ff/compiler/scanner.h>
#include <ff/compiler/type_annotation.h>
#include <unordered_map>
#include <vector>

namespace ff {
namespace ast {

class Class : public Node {
 public:
  struct Field {
    Token name;
    ff::Ref<ff::TypeAnnotation> type;
    Node* value; // nullptr - no value
    bool isConst = false;
    bool isStatic = false;

    Field(
      Token name,
      ff::Ref<ff::TypeAnnotation> type,
      Node* value = nullptr,
      bool isConst = false,
      bool isStatic = false
    );
  };

  struct Method {
    ast::Function* fn;
    bool isStatic = false;

    Method(
      ast::Function* fn,
      bool isStatic = false
    );
  };

 private:
  Token m_name;
  std::vector<Field> m_fields;
  std::vector<Method> m_methods;

 public:
  Class(Token name, std::vector<Field>& fields, std::vector<Method>& methods);
  ~Class() = default;

  Token getName() const;
  std::vector<Field>& getFields();
  std::vector<Method>& getMethods();
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_CLASS_H_ */