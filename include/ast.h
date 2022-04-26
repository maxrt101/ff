#ifndef _FF_COMPILER_AST_H_
#define _FF_COMPILER_AST_H_ 1

#include <ff/ast/node.h>
#include <ff/ast/float.h>
#include <ff/ast/integer.h>
#include <ff/ast/string.h>
#include <ff/ast/identifier.h>
#include <ff/ast/group.h>
#include <ff/ast/unary.h>
#include <ff/ast/binary.h>
#include <ff/ast/sequence.h>
#include <ff/ast/cast.h>
#include <ff/ast/function.h>
#include <ff/ast/block.h>
#include <ff/ast/var_decl.h>
#include <ff/ast/var_decl_list.h>
#include <ff/ast/call.h>
#include <ff/ast/assignment.h>
#include <ff/ast/return.h>
#include <ff/ast/print.h>
#include <ff/ast/null.h>
#include <ff/ast/true.h>
#include <ff/ast/false.h>
#include <ff/ast/if.h>
#include <ff/ast/for.h>
#include <ff/ast/while.h>
#include <ff/ast/loop.h>
#include <ff/ast/continue.h>
#include <ff/ast/break.h>
#include <ff/ast/ref.h>
#include <ff/ast/breakpoint.h>

namespace ff {
namespace ast {

void printTree(Node* node);
// void deleteTree(Node* node);

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_COMPILER_AST_H_ */