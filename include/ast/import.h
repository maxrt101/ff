#ifndef _FF_AST_IMPORT_H_
#define _FF_AST_IMPORT_H_ 1

#include <ff/ast/node.h>
#include <string>
#include <vector>

namespace ff {
namespace ast {

class Import : public Node {
 private:
  std::vector<std::string> m_imports;

 public:
  explicit Import(const std::vector<std::string>& imports);
  ~Import() = default;

  std::vector<std::string> getImports() const;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_IMPORT_H_ */