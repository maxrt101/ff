#ifndef _FF_AST_MODULE_H_
#define _FF_AST_MODULE_H_ 1

#include <ff/ast/node.h>
#include <string>

namespace ff {
namespace ast {

class Module : public Node {
 private:
  std::string m_name;
  Node* m_body;

 public:
  Module(const std::string& name, Node* body);
  ~Module() = default;

  std::string getName() const;
  Node* getBody() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_MODULE_H_ */