#ifndef _FF_COMPILER_AST_ANNOTATION_H_
#define _FF_COMPILER_AST_ANNOTATION_H_ 1

#include <ff/ast/node.h>
#include <functional>

namespace ff {

using ASTAnnotation = std::function<void(ast::Node*)>;

namespace annotations {

void print(ast::Node* node);

} /* namespace annotations */
} /* namespace ff */

#endif /* _FF_COMPILER_AST_ANNOTATION_H_ */