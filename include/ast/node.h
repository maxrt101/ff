#ifndef _FF_AST_NODE_H_
#define _FF_AST_NODE_H_ 1

#include <string>

namespace ff {
namespace ast {

enum NodeType {
  NTYPE_FLOAT_LITERAL,
  NTYPE_INTEGER_LITERAL,
  NTYPE_STRING_LITERAL,
  NTYPE_IDENTIFIER,

  NTYPE_GROUP_EXPR,
  NTYPE_UNARY_EXPR,
  NTYPE_BINARY_EXPR,

  NTYPE_SEQUENCE,

  NTYPE_FUNCTION,
  NTYPE_LAMBDA,
  NTYPE_VAR_DECL,
  NTYPE_VAR_DECL_LIST,

  NTYPE_CALL,
  NTYPE_ASSIGNMENT,
  NTYPE_RETURN,

  NTYPE_BLOCK,
  NTYPE_DICT,
  NTYPE_VECTOR,

  NTYPE_IF,
  NTYPE_FOR,
  NTYPE_FOREACH,
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
};

} /* namespace ast */
} /* namespace ff */

#endif /* _FF_AST_NODE_H_ */