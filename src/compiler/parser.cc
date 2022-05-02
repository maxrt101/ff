#include <ff/compiler/parser.h>
#include <ff/ast.h>
#include <mrt/console/colors.h>
#include <mrt/container_utils.h>

/*
program             : (top-level-decl)*
top-level-decl      : 'fn' IDENTIFIER '(' var-decl-list ')' '->' stmt
                    | 'var' var-decl ';'
var-decl-list       : var-decl (',' var-decl)+
var-decl            | IDENTIFIER ':' IDENTIFIER ('=' expr)
comma-sep-list      : expr (',' expr)*
stmt                : expr ';'
                    | 'var' var-decl ';'
                    | lvalue '=' expr ';'
                    | block
block               : '{' (expr ';')* '}'
expr                : equality
equality            : comparison (('==' | '!=') comparison)*
comparison          : term (('>' | '>=' | '<' | '<=') term)*
term                : factor (('*' | '/') factor)*
factor              : '-' factor
                    | '!' factor
                    | cast
cast                : expr AS IDENTIFIER
                    | rvalue
lvalue              : IDENTIFIER (DOT IDENTIFIER)*
                    | IDENTIFIER                            // ?
rvalue              : lvalue
                    | IDENTIFIER '(' comma-sep-list ')'
                    | 'fn' '(' var-decl-list ')' block
                    | INTEGER
                    | FLOAT
                    | STRING
*/

ff::ParseError::ParseError(Token token, const std::string& filename, const std::string& msg)
  : m_token(token), m_filename(filename), m_message(msg) {}

const char* ff::ParseError::what() const noexcept {
  return m_message.c_str();
}

void ff::ParseError::print() const {
  if (m_token.type == TOKEN_EOF) {
    printf("%s:%d: %sParseError%s at the end: %s\n", m_filename.c_str(), m_token.line, mrt::console::RED, mrt::console::RESET, m_message.c_str());
  } else {
    printf("%s:%d: %sParseError%s near '%s': %s\n", m_filename.c_str(), m_token.line, mrt::console::RED, mrt::console::RESET, m_token.str.c_str(), m_message.c_str());
  }
}

ff::Parser::Parser(const std::string& filename, const std::vector<Token>& tokens)
  : m_filename(filename), m_tokens(tokens) {
#if _FF_DEBUG_PARSER > 3
    for (auto& token : m_tokens) {
      printf("%s(%s) ", tokenTypeToString(token.type).c_str(), token.str.c_str());
    }
    putchar('\n');
#endif
  }

ff::ast::Node* ff::Parser::parse() {
  return program();
}

void ff::Parser::parserWarning(Token token, const std::string& msg) {
  if (token.type == TOKEN_EOF) {
    printf("%s:%d %sWarning%s at the end: %s\n", m_filename.c_str(), token.line, mrt::console::YELLOW, mrt::console::RESET, msg.c_str());
  } else {
    printf("%s:%d %sWarning%s near '%s': %s\n", m_filename.c_str(), token.line, mrt::console::YELLOW, mrt::console::RESET, token.str.c_str(), msg.c_str());
  }
}

void ff::Parser::syncronize() {
  advance();

  while(!isAtEnd()) {
    if (previous().type == TOKEN_SEMICOLON) return;

    switch (peek().type) {
      case TOKEN_CLASS:   [[fallthrough]];
      case TOKEN_FN:      [[fallthrough]];
      case TOKEN_VAR:     [[fallthrough]];
      case TOKEN_CONST:   [[fallthrough]];
      case TOKEN_FOR:     [[fallthrough]];
      case TOKEN_IF:      [[fallthrough]];
      case TOKEN_WHILE:   [[fallthrough]];
      case TOKEN_PRINT:   [[fallthrough]];
      case TOKEN_RETURN:
        return;
      default:
        ;
    }

    advance();
  }
}

bool ff::Parser::isAtEnd() const {
  return peek().type == TOKEN_EOF;
}

bool ff::Parser::match(const std::vector<TokenType>& types) {
  for (TokenType type : types) {
    if (check(type)) {
      advance();
      return true;
    }
  }
  return false;
}

bool ff::Parser::check(TokenType type) const {
  if (isAtEnd()) return false;
  return peek().type == type;
}

ff::Token ff::Parser::consume(TokenType type, const std::string& msg) {
  if (check(type)) return advance();
  throw ParseError(peek(), m_filename, msg);
}

ff::Token ff::Parser::advance() {
  if (!isAtEnd()) m_currentIndex++;
  return previous();
}

ff::Token ff::Parser::peek(int idx) const {
  return m_tokens[m_currentIndex + idx];
}

ff::Token ff::Parser::previous() const {
  return m_tokens[m_currentIndex - 1];
}

ff::ast::Node* ff::Parser::program() {
  std::vector<ast::Node*> nodes;
  while (peek().type == TOKEN_FN || peek().type == TOKEN_VAR || peek().type == TOKEN_CONST) {
    if (peek().type == TOKEN_FN) {
      consume(TOKEN_FN, "");
      nodes.push_back(fndecl());
    } else if (peek().type == TOKEN_VAR) {
      consume(TOKEN_VAR, "");
      nodes.push_back(vardecl());
      consume(TOKEN_SEMICOLON, "Expected ';' after var declaration");
    } else if (peek().type == TOKEN_CONST) {
      consume(TOKEN_CONST, "");
      nodes.push_back(vardecl(true));
      consume(TOKEN_SEMICOLON, "Expected ';' after const declaration");
    }
  }
  if (!isAtEnd() || peek().type != TOKEN_EOF) {
    throw ParseError(peek(), m_filename, "Unexpected token");
  }
  return new ast::Block(nodes);
}

ff::ast::Node* ff::Parser::fndecl() {
  if (!match({TOKEN_IDENTIFIER})) {
    throw ParseError(peek(), m_filename, "Expected identifier");
  }
  Token name = previous();
  ast::VarDeclList* args = nullptr;
  ast::Node* body = nullptr;
  if (!match({TOKEN_LEFT_PAREN})) {
    throw ParseError(peek(), m_filename, "Expected '('");
  }

  if (peek().type != TOKEN_RIGHT_PAREN) {
    args = varDeclList();
  }

  if (!match({TOKEN_RIGHT_PAREN})) {
    throw ParseError(peek(), m_filename, "Expected ')'");
  }

  Ref<TypeAnnotation> returnType = TypeAnnotation::create("any");
  if (match({TOKEN_COLON})) {
    returnType = typeAnnotation();
  }

  if (!match({TOKEN_RIGHT_ARROW})) { // consume?
    throw ParseError(peek(), m_filename, "Expected '->'");
  }

  if (peek().type == TOKEN_LEFT_BRACE) {
    body = block();
  } else {
    body = expression(true);
    consume(TOKEN_SEMICOLON, "Expected ';' after function body");
  }

  std::vector<Ref<TypeAnnotation>> argTypes;
  if (args) {
    for (auto varDecl : args->getList()) {
      argTypes.push_back(varDecl->getVarType());
    }
  }

  auto type = FunctionAnnotation::create(argTypes, returnType);

  return new ast::Function(name, args, type, body);
}

ff::ast::Node* ff::Parser::vardecl(bool isConst) {
  if (!match({TOKEN_IDENTIFIER})) {
    throw ParseError(peek(), m_filename, "Expected an identifier after 'var'");
  }
  Token name = previous();
  Ref<TypeAnnotation> type = TypeAnnotation::any();
  ast::Node* value = nullptr;
  if (match({TOKEN_COLON})) {
    type = typeAnnotation();
  }
  if (match({TOKEN_EQUAL})) {
    value = expression(true);
  }
  return new ast::VarDecl(name, type, value, isConst);
}

ff::ast::Node* ff::Parser::ifstmt() {
  if (!match({TOKEN_LEFT_PAREN})) {
    throw ParseError(peek(), m_filename, "Expected a '(' after 'if'");
  }
  ast::Node* condition = expression(true);
  if (!match({TOKEN_RIGHT_PAREN})) {
    throw ParseError(peek(), m_filename, "Expected a ')' after a condition");
  }
  ast::Node* body = nullptr;
  if (peek().type == TOKEN_LEFT_BRACE) {
    body = block();
  } else {
    body = expression(false);
    consume(TOKEN_SEMICOLON, "Expected ';' after an expression");
  }
  ast::Node* elseBody = nullptr;
  if (match({TOKEN_ELSE})) {
    if (peek().type == TOKEN_LEFT_BRACE) {
      elseBody = block();
    } else if (peek().type == TOKEN_IF) {
      consume(TOKEN_IF);
      elseBody = ifstmt();
    } else {
      elseBody = expression(false);
      consume(TOKEN_SEMICOLON, "Expected ';' after an expression");
    }
  }
  return new ast::If(condition, body, elseBody);
}

ff::ast::Node* ff::Parser::forstmt() {
  if (peek().type == TOKEN_LEFT_PAREN) { // for
    consume(TOKEN_LEFT_PAREN);
    
    ast::Node* init = nullptr;
    ast::Node* cond = nullptr;
    ast::Node* incr = nullptr;
    
    if (peek().type != TOKEN_SEMICOLON) {
      if (peek().type == TOKEN_VAR) {
        consume(TOKEN_VAR);
        init = vardecl();
      } else {
        init = expression(true);
      }
    }

    if (!match({TOKEN_SEMICOLON})) {
      throw ParseError(peek(), m_filename, "Expected ';' after 'for' initializer");
    }

    if (peek().type != TOKEN_SEMICOLON) {
      cond = expression(true);
    }

    if (!match({TOKEN_SEMICOLON})) {
      throw ParseError(peek(), m_filename, "Expected ';' after 'for' condition");
    }

    if (peek().type != TOKEN_RIGHT_PAREN) {
      incr = expression(true);
    }

    if (!match({TOKEN_RIGHT_PAREN})) {
      throw ParseError(peek(), m_filename, "Expected ')' after 'for' increment");
    }

    ast::Node* body = nullptr;
    if (peek().type == TOKEN_LEFT_BRACE) {
      body = block();
    } else {
      body = expression(true);
      if (!match({TOKEN_SEMICOLON})) {
        throw ParseError(peek(), m_filename, "Expected ')' after 'for' body");
      }
    }

    return new ast::For(init, cond, incr, body);
  } else if (peek().type == TOKEN_IDENTIFIER) { // foreach
    ast::Node* var = vardecl();
    if (!match({TOKEN_IN})) {
      throw ParseError(peek(), m_filename, "Expected 'in' after variable declaration in foreach");
    }

    ast::Node* iter = expression(true);
    if (peek().type != TOKEN_LEFT_BRACE) {
      throw ParseError(peek(), m_filename, "Expected '{' after foreach iter");
    }

    ast::Node* body = block();
    return new ast::ForEach(var, iter, body);
  } else {
    throw ParseError(peek(), m_filename, "Expected '(' or identifier after 'for'");
  }
  return nullptr;
}

ff::ast::Node* ff::Parser::whilestmt() {
  if (!match({TOKEN_LEFT_PAREN})) {
    throw ParseError(peek(), m_filename, "Expected '(' after 'while'");
  }

  ast::Node* cond = expression(true);

  if (!match({TOKEN_RIGHT_PAREN})) {
    throw ParseError(peek(), m_filename, "Expected ')' after 'while' condition");
  }

  ast::Node* body = nullptr;
  if (peek().type == TOKEN_LEFT_BRACE) {
    body = block();
  } else {
    body = expression(true);
    if (!match({TOKEN_SEMICOLON})) {
      throw ParseError(peek(), m_filename, "Expected ')' after 'while' body");
    }
  }

  return new ast::While(cond, body);
}

ff::ast::Node* ff::Parser::loopstmt() {
  return new ast::Loop(block());
}

ff::ast::Node* ff::Parser::statement() {
  switch (peek().type) {
    case TOKEN_LEFT_BRACE:
      return block();
    case TOKEN_IDENTIFIER: {
      ast::Node* value = lvalue(false);
      // NOTE: setIsReturnValueExpected patches expectance of value if that value is used in call or assignment
      if (peek().type == TOKEN_LEFT_PAREN) {
        consume(TOKEN_LEFT_PAREN);
        ((ast::Call*)value)->setIsReturnValueExpected(true);
        value = call(value, false);
      } else if (peek().type == TOKEN_EQUAL) {
        consume(TOKEN_EQUAL);
        ((ast::Call*)value)->setIsReturnValueExpected(true);
        value = new ast::Assignment(value, expression(true));
      } else if (peek().type == TOKEN_COLON_EQUAL) {
        consume(TOKEN_COLON_EQUAL);
        ((ast::Call*)value)->setIsReturnValueExpected(true);
        value = new ast::Assignment(value, expression(true), true);
      } /*else {
        throw ParseError(peek(), m_filename, "Expected call or assignment");
      }*/
      return value;
    }
    case TOKEN_VAR:
      consume(TOKEN_VAR);
      return vardecl();
    case TOKEN_CONST:
      consume(TOKEN_CONST);
      return vardecl(true);
    case TOKEN_RETURN:
      consume(TOKEN_RETURN);
      return new ast::Return(expression(true));
    case TOKEN_IF:
      consume(TOKEN_IF);
      return ifstmt();
    case TOKEN_FOR:
      consume(TOKEN_FOR);
      return forstmt();
    case TOKEN_WHILE:
      consume(TOKEN_WHILE);
      return whilestmt();
    case TOKEN_LOOP:
      consume(TOKEN_LOOP);
      return loopstmt();
    case TOKEN_CONTINUE:
      consume(TOKEN_CONTINUE);
      return new ast::Continue();
    case TOKEN_BREAK:
      consume(TOKEN_BREAK);
      return new ast::Break();
    case TOKEN_CLASS:
      throw ParseError(peek(), m_filename, "Unimplemented");
    case TOKEN_PRINT:
      consume(TOKEN_PRINT);
      return new ast::Print(expression(true));
#ifdef _DEBUG
    case TOKEN_BREAKPOINT: {
      consume(TOKEN_BREAKPOINT);
      return new ast::Breakpoint();
    }
#endif
    default:
      return expression(true);
  }
}

ff::ast::Node* ff::Parser::block() {
  std::vector<ast::Node*> result;
  consume(TOKEN_LEFT_BRACE, "Expected '{' at the begining of the block");
  if (peek().type != TOKEN_RIGHT_BRACE) {
    result = statementList();
  }
  consume(TOKEN_RIGHT_BRACE, "Expected '}' at the end of the block");

  return new ast::Block(result);
}

std::vector<ff::ast::Node*> ff::Parser::statementList() {
  std::vector<ff::ast::Node*> nodes;

  while (1) {
    if (peek().type == TOKEN_RIGHT_BRACE) break;
    nodes.push_back(statement());
    /* Skip semicolon after blocks, if's, for's, and while's */
    if (mrt::isIn(nodes.back()->getType(), ast::NTYPE_BLOCK, ast::NTYPE_IF, ast::NTYPE_FOR, ast::NTYPE_FOREACH, ast::NTYPE_WHILE, ast::NTYPE_LOOP)) continue;
    if (!match({TOKEN_SEMICOLON})) break;
  }

  if (peek().type != TOKEN_RIGHT_BRACE) {
    throw ParseError(peek(), m_filename, "Expected ';' after a statement");
  }

  return nodes;
}

std::vector<ff::ast::Node*> ff::Parser::expressionList(bool isReturnValueExpected) {
  std::vector<ast::Node*> nodes {expression(isReturnValueExpected)};
  while (match({TOKEN_COMMA})) {
    nodes.push_back(expression(isReturnValueExpected));
  }
  return nodes;
}

ff::ast::VarDeclList* ff::Parser::varDeclList() {
  std::vector<ast::VarDecl*> nodes {(ast::VarDecl*)vardecl()};
  while (match({TOKEN_COMMA})) {
    nodes.push_back((ast::VarDecl*)vardecl());
  }
  return new ast::VarDeclList(nodes);
}

ff::ast::Node* ff::Parser::call(ast::Node* callee, bool isReturnValueExpected) {
  std::vector<ast::Node*> args;
  if (peek().type != TOKEN_RIGHT_PAREN) {
    args = expressionList(true);
  }
  consume(TOKEN_RIGHT_PAREN, "Expected ')' after function call");
  return new ast::Call(callee, args, isReturnValueExpected);
}

ff::ast::Node* ff::Parser::lambda() {
  ast::VarDeclList* args = nullptr;
  ast::Node* body = nullptr;
  if (!match({TOKEN_LEFT_PAREN})) {
    throw ParseError(peek(), m_filename, "Expected '('");
  }

  if (peek().type != TOKEN_RIGHT_PAREN) {
    args = varDeclList();
  }

  if (!match({TOKEN_RIGHT_PAREN})) {
    throw ParseError(peek(), m_filename, "Expected ')'");
  }

  Ref<TypeAnnotation> returnType = TypeAnnotation::create("any");
  if (match({TOKEN_COLON})) {
    returnType = typeAnnotation();
  }

  if (!match({TOKEN_RIGHT_ARROW})) { // consume?
    throw ParseError(peek(), m_filename, "Expected '->'");
  }

  if (peek().type == TOKEN_LEFT_BRACE) {
    body = block();
  } else {
    body = expression(true);
  }

  std::vector<Ref<TypeAnnotation>> argTypes;
  if (args) {
    for (auto varDecl : args->getList()) {
      argTypes.push_back(varDecl->getVarType());
    }
  }

  auto type = FunctionAnnotation::create(argTypes, returnType);

  return new ast::Lambda(args, type, body);
}

ff::ast::Node* ff::Parser::expression(bool isReturnValueExpected) {
  return logic(isReturnValueExpected);
}

ff::ast::Node* ff::Parser::logic(bool isReturnValueExpected) {
  ast::Node* expr = equality(isReturnValueExpected);

  while (match({TOKEN_AND, TOKEN_OR})) {
    Token op = previous();
    ast::Node* right = equality(isReturnValueExpected);
    expr = new ast::Binary(op, expr, right);
  }

  return expr;
}

ff::ast::Node* ff::Parser::equality(bool isReturnValueExpected) {
  ast::Node* expr = comparison(isReturnValueExpected);

  while (match({TOKEN_BANG_EQUAL, TOKEN_EQUAL_EQUAL})) {
    Token op = previous();
    ast::Node* right = comparison(isReturnValueExpected);
    expr = new ast::Binary(op, expr, right);
  }

  return expr;
}

ff::ast::Node* ff::Parser::comparison(bool isReturnValueExpected) {
  ast::Node* expr = term(isReturnValueExpected);

  while (match({TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL})) {
    Token op = previous();
    ast::Node* right = term(isReturnValueExpected);
    expr = new ast::Binary(op, expr, right);
  }

  return expr;
}

ff::ast::Node* ff::Parser::term(bool isReturnValueExpected) {
  ast::Node* expr = factor(isReturnValueExpected);

  while (match({TOKEN_MINUS, TOKEN_PLUS})) {
    Token op = previous();
    ast::Node* right = factor(isReturnValueExpected);
    expr = new ast::Binary(op, expr, right);
  }

  return expr;
}

ff::ast::Node* ff::Parser::factor(bool isReturnValueExpected) {
  ast::Node* expr = unary(isReturnValueExpected);

  while (match({TOKEN_SLASH, TOKEN_STAR})) {
    Token op = previous();
    ast::Node* right = unary(isReturnValueExpected);
    expr = new ast::Binary(op, expr, right);
  }

  return expr;
}

ff::ast::Node* ff::Parser::unary(bool isReturnValueExpected) {
  if (match({TOKEN_BANG, TOKEN_MINUS, TOKEN_INCREMENT, TOKEN_DECREMENT})) {
    Token op = previous();
    ast::Node* expr = unary(isReturnValueExpected);
    return new ast::Unary(op, expr);
  }

  return cast(isReturnValueExpected);
}

ff::ast::Node* ff::Parser::cast(bool isReturnValueExpected) {
  ast::Node* expr = rvalue(isReturnValueExpected);

  if (match({TOKEN_AS})) {
    return new ast::Cast(typeAnnotation(), expr);
  }

  return expr;
}

ff::ast::Node* ff::Parser::rvalue(bool isReturnValueExpected) {
  if (match({TOKEN_NUMBER})) {
    if (previous().str.find('.') != std::string::npos) {
      return new ast::FloatLiteral(previous());
    } else {
      return new ast::IntegerLiteral(previous());
    }
  }

  if (match({TOKEN_STRING})) {
    return new ast::StringLiteral(previous());
  }

  if (match({TOKEN_NULL})) {
    return new ast::Null();
  }

  if (match({TOKEN_TRUE})) {
    return new ast::True();
  }

  if (match({TOKEN_FALSE})) {
    return new ast::False();
  }

  if (match({TOKEN_LEFT_PAREN})) {
    ast::Node* expr = expression(isReturnValueExpected);
    consume(TOKEN_RIGHT_PAREN, "Expected ')' after an expression");
    return new ast::Group(expr);
  }

  if (match({TOKEN_REF})) {
    ast::Node* lval = lvalue(isReturnValueExpected);
    return new ast::Ref(lval);
  }

  if (match({TOKEN_FN})) {
    return lambda();
  }

  ast::Node* lval = lvalue(isReturnValueExpected);

  return lval;
}

ff::ast::Node* ff::Parser::lvalue(bool isReturnValueExpected) {
  std::vector<ast::Node*> nodes;
  do {
    if (peek().type != TOKEN_IDENTIFIER) {
      break;
    }
    auto id = advance();
    if (match({TOKEN_LEFT_PAREN})) {
      // NOTE: Return value is expected only if there is next chain element
      ast::Call* callNode = (ast::Call*)call(new ast::Identifier(id), false);
      if (peek().type == TOKEN_DOT) {
        callNode->setIsReturnValueExpected(true);
      }
      nodes.push_back(callNode);
    } else {
      nodes.push_back(new ast::Identifier(id));
    }
  } while (match({TOKEN_DOT}));
  // NOTE: Patch return value expectance based on environment
  if (nodes.size() == 1) {
    if (nodes.front()->getType() == ast::NTYPE_CALL) {
      ((ast::Call*)nodes.front())->setIsReturnValueExpected(isReturnValueExpected);
    }
    return nodes[0];
  } else if (nodes.size() > 1) {
    if (nodes.back()->getType() == ast::NTYPE_CALL) {
      ((ast::Call*)nodes.back())->setIsReturnValueExpected(isReturnValueExpected);
    }
    return new ast::Sequence(nodes);
  }

  // FIXME: Can happen if keyword is identified as an identifier (see scanner.cc)
  printf("DEV: CHECK FOR KEYWORDS\n");
  throw ParseError(peek(), m_filename, "Expected an expression");
}

ff::Ref<ff::TypeAnnotation> ff::Parser::typeAnnotation() {
  auto consumeType = [&]() -> std::string {
    std::string result;
    do {
      if (peek().type != TOKEN_IDENTIFIER) {
        break;
      }
      auto id = advance();
      if (id.type == TOKEN_IDENTIFIER) {
        if (!result.empty()) result += ".";
        result += id.str;
      } else {
        throw ParseError(id, m_filename, "Unexpected token");
      }
    } while (match({TOKEN_DOT}));
    return result;
  };

  if (peek().type == TOKEN_IDENTIFIER) {
    auto id = TypeAnnotation::create(consumeType(), true);
    if (peek().type == TOKEN_VBAR) {
      std::vector<Ref<TypeAnnotation>> types {id};
      while (peek().type == TOKEN_VBAR) {
        consume(TOKEN_VBAR);
        if (peek().type == TOKEN_IDENTIFIER) {
          types.push_back(TypeAnnotation::create(consumeType(), true));
        } else {
          throw ParseError(peek(), m_filename, "Expected an identifier");
        }
      }
      return UnionAnnotation::create(types, true).asRefTo<TypeAnnotation>();
    } else {
      return id;
    }
  } else if (peek().type == TOKEN_LEFT_PAREN) {
    consume(TOKEN_LEFT_PAREN);
    std::vector<Ref<TypeAnnotation>> args;
    Ref<TypeAnnotation> returnType = TypeAnnotation::create("any");

    if (peek().type != TOKEN_RIGHT_PAREN) {
      args.push_back(typeAnnotation());
      while (match({TOKEN_COMMA})) {
        args.push_back(typeAnnotation());
      }
    }

    if (!match({TOKEN_RIGHT_PAREN})) {
      throw ParseError(peek(), m_filename, "Expected ')'");
    }

    if (!match({TOKEN_RIGHT_ARROW})) {
      throw ParseError(peek(), m_filename, "Expected '->'");
    }
    returnType = typeAnnotation();

    return FunctionAnnotation::create(args, returnType).asRefTo<TypeAnnotation>();
  }

  throw ParseError(peek(), m_filename, "Unrecognized type annotation");
}
