#ifndef _FF_COMPILER_PARSER_H_
#define _FF_COMPILER_PARSER_H_ 1

#include <ff/compiler/type_annotation.h>
#include <ff/compiler/token.h>
#include <ff/errors.h>
#include <ff/ast.h>
#include <exception>
#include <vector>
#include <string>

namespace ff {

class Parser {
 private:
  std::string m_filename;
  std::vector<Token> m_tokens;
  ast::Node* m_root = nullptr;
  int m_currentIndex = 0;
  bool m_hadError = false;
  bool m_panicMode = false;

 public:
  Parser(const std::string& filename, const std::vector<Token>& tokens);

  ast::Node* parse();

 private:
  // Utility for Tokens
  bool isAtEnd() const;
  bool match(const std::vector<TokenType>& types);
  bool check(TokenType type) const;
  Token consume(TokenType type, const std::string& msg = "Unecpected token");
  Token advance();
  Token peek(int idx = 0) const;
  Token previous() const;

  // Grammar parsing functions
  ast::Node* program(bool checkEnd = true);
  ast::Node* module();
  ast::Node* import();
  ast::Node* fndecl();
  ast::Node* classdecl();
  void classbody(std::vector<ast::Class::Field>& fields, std::vector<ast::Class::Method>& methods);
  ast::Node* vardecl(bool isConst = false);
  ast::Node* statement(bool isInOtherStatement = false);
  ast::Node* ifstmt();
  ast::Node* forstmt();
  ast::Node* whilestmt();
  ast::Node* loopstmt();
  ast::Node* block();
  std::vector<ast::Node*> statementList();
  std::vector<ast::Node*> expressionList(bool isReturnValueExpected);
  ast::VarDeclList* varDeclList();
  ast::Node* call(ast::Node* callee, bool isReturnValueExpected);
  ast::Node* lambda();
  ast::Node* initializer(bool isReturnValueExpected);
  ast::Node* expression(bool isReturnValueExpected);
  ast::Node* logic(bool isReturnValueExpected);
  ast::Node* equality(bool isReturnValueExpected);
  ast::Node* comparison(bool isReturnValueExpected);
  ast::Node* term(bool isReturnValueExpected);
  ast::Node* factor(bool isReturnValueExpected);
  ast::Node* unary(bool isReturnValueExpected);
  ast::Node* cast(bool isReturnValueExpected);
  ast::Node* newexpr(bool isReturnValueExpected);
  ast::Node* rvalue(bool isReturnValueExpected);
  ast::Node* lvalue(bool isReturnValueExpected, bool allowCall = true);

  Ref<TypeAnnotation> typeAnnotation();
};

} /* namespace ff */

#endif /* _FF_COMPILER_PARSER_H_ */