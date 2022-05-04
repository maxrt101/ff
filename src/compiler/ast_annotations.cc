#include <ff/compiler/ast_annotation.h>
#include <ff/errors.h>
#include <ff/ast.h>

void ff::annotations::print(ast::Node* node) {
  printTree(node);
}
