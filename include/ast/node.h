#ifndef _FF_AST_NODE_H_
#define _FF_AST_NODE_H_ 1

#include <string>

namespace ff {
namespace ast {

enum NodeType {
  NTYPE_FLOAT_LITERAL,    // float
  NTYPE_INTEGER_LITERAL,  // int
  NTYPE_STRING_LITERAL,   // str
  NTYPE_IDENTIFIER,       // id

  NTYPE_GROUP_EXPR,       // '(' expr ')'
  NTYPE_UNARY_EXPR,       // OP expr
  NTYPE_BINARY_EXPR,      // expr OP expr

  NTYPE_SEQUENCE,         // (id|call) ('.' (id|call))*

  NTYPE_FUNCTION,
  NTYPE_VAR_DECL,
  NTYPE_VAR_DECL_LIST,

  NTYPE_CALL,
  NTYPE_ASSIGNMENT,
  NTYPE_RETURN,

  NTYPE_BLOCK,

  NTYPE_IF,
  NTYPE_FOR,
  NTYPE_WHILE,
  NTYPE_LOOP,

  NTYPE_EXPR_LIST_EXPR,
  NTYPE_CAST_EXPR,

  NTYPE_NULL,
  NTYPE_TRUE,
  NTYPE_FALSE,

  NTYPE_CONTINUE,
  NTYPE_BREAK,

  NTYPE_PRINT,
  NTYPE_REF,
  NTYPE_BREAKPOINT,
};

std::string nodeTypeToString(NodeType type);

class Node {
 private:
  NodeType m_type;

 public:
  Node(NodeType type);
  virtual ~Node();

  NodeType getType() const;
  virtual std::string toString() const;

  template <typename T>
  inline T* as() {
    return (T*)this;
  }

  // virtual void interpret() = 0;
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_NODE_H_ */