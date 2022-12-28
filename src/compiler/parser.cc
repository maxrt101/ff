#include <ff/compiler/parser.h>
#include <ff/utils/macros.h>
#include <ff/ast.h>
#include <ff/log.h>

#include <mrt/console/colors.h>
#include <mrt/container_utils.h>

#include <algorithm>
#include <vector>
#include <string>


ff::ParseError::ParseError(const Token& token, const std::string& filename, const std::string& msg)
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

bool ff::Parser::isAtEnd() const {
  return peek().type == TOKEN_EOF;
}

bool ff::Parser::match(const std::vector<TokenType>& types) {
  if (std::any_of(BEGIN_END(types), [&](auto type) { return check(type); })) {
    advance();
    return true;
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

ff::ast::Node* ff::Parser::program(bool checkEnd) {
  std::vector<ast::Node*> nodes;
  while (mrt::isIn(peek().type, TOKEN_FN, TOKEN_CLASS, TOKEN_VAR, TOKEN_CONST, TOKEN_MODULE, TOKEN_IMPORT)) {
    if (peek().type == TOKEN_MODULE) {
      consume(TOKEN_MODULE);
      nodes.push_back(module());
    } else if (peek().type == TOKEN_IMPORT) {
      consume(TOKEN_IMPORT);
      nodes.push_back(import());
    } else if (peek().type == TOKEN_FN) {
      consume(TOKEN_FN);
      nodes.push_back(fndecl());
    } else if (peek().type == TOKEN_CLASS) {
      consume(TOKEN_CLASS);
      nodes.push_back(classdecl());
    } else if (peek().type == TOKEN_VAR) {
      consume(TOKEN_VAR);
      nodes.push_back(vardecl());
      consume(TOKEN_SEMICOLON, "Expected ';' after var declaration");
    } else if (peek().type == TOKEN_CONST) {
      consume(TOKEN_CONST);
      nodes.push_back(vardecl(true));
      consume(TOKEN_SEMICOLON, "Expected ';' after const declaration");
    }

  }
  if (checkEnd && (!isAtEnd() || peek().type != TOKEN_EOF)) {
    throw ParseError(peek(), m_filename, "Unexpected token");
  }
  return new ast::Block(nodes);
}

ff::ast::Node* ff::Parser::module() {
  if (!match({TOKEN_IDENTIFIER})) {
    throw ParseError(peek(), m_filename, "Expected identifier");
  }
  std::string name = previous().str;
  consume(TOKEN_LEFT_BRACE, "Expected '{' at the beginning of the module");
  ast::Node* body = program(false);
  consume(TOKEN_RIGHT_BRACE, "Expected '}' at the end of the module");
  return new ast::Module(name, body);
}

ff::ast::Node* ff::Parser::import() {
  std::vector<std::string> imports;

  auto getImport = [&]() {
    if (!match({TOKEN_STRING})) {
      throw ParseError(peek(), m_filename, "Import name must be a string");
    }
    return previous().str;
  };

  if (match({TOKEN_LEFT_BRACE})) {
    imports.push_back(getImport());
    while (match({TOKEN_COMMA})) {
      imports.push_back(getImport());
    }
    if (!match({TOKEN_RIGHT_BRACE})) {
      throw ParseError(peek(), m_filename, "Expected '}' after multiple imports");
    }
  } else {
    imports.push_back(getImport());
    if (!match({TOKEN_SEMICOLON})) {
      throw ParseError(peek(), m_filename, "Expected ';' after single import");
    }
  }

  return new ast::Import(imports);
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
    std::transform(
      BEGIN_END(args->getList()),
      std::back_inserter(argTypes),
      [](auto varDecl) {
        return varDecl->getVarType();
      }
    );
  }

  auto type = FunctionAnnotation::create(argTypes, returnType);

  return new ast::Function(name, args, type, body);
}

ff::ast::Node* ff::Parser::classdecl() {
  if (!match({TOKEN_IDENTIFIER})) {
    throw ParseError(peek(), m_filename, "Expected identifier");
  }
  Token name = previous();

  if (!match({TOKEN_LEFT_BRACE})) {
    throw ParseError(peek(), m_filename, "Expected '{' after class name");
  }

  std::vector<ast::Class::Field> fields;
  std::vector<ast::Class::Method> methods;
  classbody(fields, methods);

  if (!match({TOKEN_RIGHT_BRACE})) {
    throw ParseError(peek(), m_filename, "Expected '}' after class body");
  }

  return new ast::Class(name, fields, methods);
}

void ff::Parser::classbody(std::vector<ast::Class::Field>& fields, std::vector<ast::Class::Method>& methods) {
  auto processField = [&](bool isConst, bool isStatic) -> ast::Class::Field {
    Token name = previous();
    if (!match({TOKEN_COLON})) {
      throw ParseError(peek(), m_filename, "Expected ':' after field name");
    }
    Ref<TypeAnnotation> type = typeAnnotation();
    ast::Node* value = nullptr;
    if (match({TOKEN_EQUAL})) {
      value = expression(true);
    }
    if (!match({TOKEN_SEMICOLON})) {
      throw ParseError(peek(), m_filename, "Expected ';' after field declaration");
    }

    return ast::Class::Field(name, type, value, isConst, isStatic);
  };

  auto processMethod = [&](bool isStatic) -> ast::Class::Method {    
    return ast::Class::Method((ast::Function*)fndecl(), isStatic);
  };

  while (peek().type != TOKEN_RIGHT_BRACE) {
    if (match({TOKEN_IDENTIFIER})) {
      fields.push_back(processField(false, false));
    } else if (match({TOKEN_FN})) {
      methods.push_back(processMethod(false));
    // TODO: static & const check
    } else {
      throw ParseError(peek(), m_filename, "Expected field or method declaration");
    }
  }
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
  ast::Node* body = statement(true);
  if (body->getType() != ast::NTYPE_BLOCK) {
    consume(TOKEN_SEMICOLON, "Expected ';' after an expression");
  }
  ast::Node* elseBody = nullptr;
  if (match({TOKEN_ELSE})) {
    if (peek().type == TOKEN_IF) {
      consume(TOKEN_IF);
      elseBody = ifstmt();
    } else {
      elseBody = statement(true);
      if (elseBody->getType() != ast::NTYPE_BLOCK) {
        consume(TOKEN_SEMICOLON, "Expected ';' after an expression");
      }
    }
  }
  return new ast::If(condition, body, elseBody);
}

ff::ast::Node* ff::Parser::forstmt() {
  if (peek().type == TOKEN_LEFT_PAREN) {
    consume(TOKEN_LEFT_PAREN);
    
    ast::Node *init = nullptr, *cond = nullptr, *incr = nullptr;
    
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

    ast::Node* body = statement(true);
    if (body->getType() != ast::NTYPE_BLOCK) {
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

  ast::Node* body = statement(true);
  if (body->getType() != ast::NTYPE_BLOCK) {
    if (!match({TOKEN_SEMICOLON})) {
      throw ParseError(peek(), m_filename, "Expected ')' after 'while' body");
    }
  }

  return new ast::While(cond, body);
}

ff::ast::Node* ff::Parser::loopstmt() {
  return new ast::Loop(block());
}

ff::ast::Node* ff::Parser::statement(bool isInOtherStatement) {
  if (isInOtherStatement) {
    if (peek().type == TOKEN_VAR || peek().type == TOKEN_CONST) {
      throw ParseError(peek(), m_filename, "Variable declaration is not allowed here");
    } else if (peek().type == TOKEN_CLASS) {
      throw ParseError(peek(), m_filename, "Class declaration is not allowed here");
    }
  }
  switch (peek().type) {
    case TOKEN_LEFT_BRACE:
      return block();
    case TOKEN_IDENTIFIER: {
      ast::Node* value = lvalue(false);
      // NOTE: setIsReturnValueExpected patches expectance of value if that value is used in call or assignment
      if (peek().type == TOKEN_LEFT_PAREN) {
        consume(TOKEN_LEFT_PAREN);
        if (value->getType() == ast::NTYPE_CALL) {
          ((ast::Call*)value)->setIsReturnValueExpected(true);
        }
        value = call(value, false);
      } else if (peek().type == TOKEN_EQUAL) {
        consume(TOKEN_EQUAL);
        if (value->getType() == ast::NTYPE_CALL) {
          ((ast::Call*)value)->setIsReturnValueExpected(true);
        }
        value = new ast::Assignment(value, expression(true));
      } else if (peek().type == TOKEN_COLON_EQUAL) {
        consume(TOKEN_COLON_EQUAL);
        if (value->getType() == ast::NTYPE_CALL) {
          ((ast::Call*)value)->setIsReturnValueExpected(true);
        }
        value = new ast::Assignment(value, expression(true), true);
      } /* else {
        throw ParseError(peek(), m_filename, "Expected call or assignment");
      } */
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
      consume(TOKEN_CLASS);
      return classdecl();
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
      return expression(false);
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
    /* Skip semicolon after blocks, if's, and loops */
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
    std::transform(
      BEGIN_END(args->getList()),
      std::back_inserter(argTypes),
      [](auto varDecl) {
        return varDecl->getVarType();
      }
    );
  }

  auto type = FunctionAnnotation::create(argTypes, returnType);

  return new ast::Lambda(args, type, body);
}

ff::ast::Node* ff::Parser::initializer(bool isReturnValueExpected) {
  ast::NodeType type = ast::NTYPE_DICT;
  std::map<std::string, ast::Node*> fields;
  std::vector<ast::Node*> elements;

  auto getFirst = [&]() {
    if (peek().type != TOKEN_STRING) {
      type = ast::NTYPE_VECTOR;
      elements.push_back(expression(isReturnValueExpected));
    } else {
      std::string key = peek().str;
      consume(TOKEN_STRING);
      if (!match({TOKEN_RIGHT_ARROW})) {
        type = ast::NTYPE_VECTOR;
        elements.push_back(new ast::StringLiteral(previous()));
      } else {
        fields[key] = expression(isReturnValueExpected);
      }
    }
  };

  auto getNext = [&]() {
    if (type == ast::NTYPE_DICT) {
      if (peek().type != TOKEN_STRING) {
        throw ParseError(peek(), m_filename, "Expected string as key");
      }
      std::string key = peek().str;
      consume(TOKEN_STRING);
      if (!match({TOKEN_RIGHT_ARROW})) {
        throw ParseError(peek(), m_filename, "Expected '->'");
      }
      fields[key] = expression(isReturnValueExpected);
    } else {
      elements.push_back(expression(isReturnValueExpected));
    }
  };

  getFirst();
  while (match({TOKEN_COMMA})) {
    getNext();
  }

  if (!match({TOKEN_RIGHT_BRACE})) {
    throw ParseError(peek(), m_filename, "Expected '}'");
  }

  if (type == ast::NTYPE_DICT) {
    return new ast::Dict(fields);
  } else {
    return new ast::Vector(elements);
  }
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

// TODO: Add & and | in term or factor 

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

  while (match({TOKEN_SLASH, TOKEN_STAR, TOKEN_PERCENT})) {
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

ff::ast::Node* ff::Parser::newexpr(bool isReturnValueExpected) {
  ast::Node* expr = lvalue(true, false);

  bool isConstructorCalled = false;
  std::vector<ast::Node*> constructorArgs;

  if (match({TOKEN_LEFT_PAREN})) {
    isConstructorCalled = true;
    if (peek().type != TOKEN_RIGHT_PAREN) {
      constructorArgs = expressionList(true);
    }
    consume(TOKEN_RIGHT_PAREN, "Expected ')' after constructor args");
  }

  return new ast::New(expr, isConstructorCalled, constructorArgs);
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

  if (match({TOKEN_NEW})) {
    return newexpr(isReturnValueExpected);
  }

  if (match({TOKEN_LEFT_BRACE})) {
    return initializer(isReturnValueExpected);
  }

  ast::Node* lval = lvalue(isReturnValueExpected);

  return lval;
}

ff::ast::Node* ff::Parser::lvalue(bool isReturnValueExpected, bool allowCall) {
  std::vector<ast::Node*> nodes;
  do {
    if (peek().type != TOKEN_IDENTIFIER) {
      break;
    }
    auto id = advance();
    if (allowCall && match({TOKEN_LEFT_PAREN})) {
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
  warning("For developer: Check keywords\n");
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
  } else if (peek().type == TOKEN_REF) {
    consume(TOKEN_REF);
    Ref<TypeAnnotation> annotation = typeAnnotation();
    annotation = annotation->copy();
    annotation->isRef = true;
    return annotation;
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
