#ifndef _FF_AST_DICT_H_
#define _FF_AST_DICT_H_ 1

#include <ff/ast/node.h>
#include <string>
#include <map>

namespace ff {
namespace ast {

class Dict : public Node {
 private:
  std::map<std::string, Node*> m_fields;

 public:
  Dict(std::map<std::string, Node*> fields);
  ~Dict() = default;

  std::map<std::string, Node*> getFields() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_DICT_H_ */