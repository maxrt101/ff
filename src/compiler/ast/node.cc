#include <ff/ast/node.h>
#include <map>

static std::map<ff::ast::NodeType, std::string> g_nodeTypes {
  {ff::ast::NTYPE_FLOAT_LITERAL, "FloatLiteral"},
  {ff::ast::NTYPE_INTEGER_LITERAL, "IntegerLiteral"},
  {ff::ast::NTYPE_STRING_LITERAL, "StringLiteral"},
  {ff::ast::NTYPE_IDENTIFIER, "Identifier"},
  {ff::ast::NTYPE_SEQUENCE, "Sequence"},
  {ff::ast::NTYPE_GROUP_EXPR, "Group"},
  {ff::ast::NTYPE_UNARY_EXPR, "Unary"},
  {ff::ast::NTYPE_BINARY_EXPR, "Binary"},
  {ff::ast::NTYPE_BLOCK, "Block"},
  {ff::ast::NTYPE_EXPR_LIST_EXPR, "ExprList"},
  {ff::ast::NTYPE_VAR_DECL, "VarDecl"},
  {ff::ast::NTYPE_VAR_DECL_LIST, "VarDeclList"},
  {ff::ast::NTYPE_FUNCTION, "Function"},
  {ff::ast::NTYPE_CAST_EXPR, "Cast"},
  {ff::ast::NTYPE_CALL, "Call"},
  {ff::ast::NTYPE_ASSIGNMENT, "Assignment"},
  {ff::ast::NTYPE_RETURN, "Return"},
  {ff::ast::NTYPE_PRINT, "Print"},
  {ff::ast::NTYPE_REF, "Ref"},
};

std::string ff::ast::nodeTypeToString(NodeType type) {
  auto itr = g_nodeTypes.find(type);
  if (itr != g_nodeTypes.end()) {
    return itr->second;
  }
  return "<unknown>";
}

ff::ast::Node::Node(NodeType type) : m_type(type) {}

ff::ast::Node::~Node() {}

ff::ast::NodeType ff::ast::Node::getType() const {
  return m_type;
}

void ff::ast::Node::addAnnotation(const std::string& annotation) {
  m_annotations.push_back(annotation);
}

void ff::ast::Node::addAnnotations(const std::vector<std::string>& annotations) {
  m_annotations.insert(m_annotations.end(), annotations.begin(), annotations.end());
}

std::vector<std::string>& ff::ast::Node::getAnnotations() {
  return m_annotations;
}

std::string ff::ast::Node::toString() const {
  return "Node<" + nodeTypeToString(getType()) + ">";
}
