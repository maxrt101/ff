#include <ff/compiler/compiler.h>
#include <ff/compiler/scanner.h>
#include <ff/compiler/parser.h>
#include <ff/utils/macros.h>
#include <ff/utils/path.h>
#include <ff/utils/str.h>
#include <ff/memory.h>
#include <ff/config.h>
#include <ff/types.h>
#include <ff/log.h>

#include <mrt/container_utils.h>
#include <mrt/console/colors.h>
#include <mrt/strutils.h>

#include <functional>
#include <algorithm>
#include <sstream>
#include <climits>
#include <cstdarg>

ff::Compiler::Variable::Variable(const std::string& name, Ref<TypeAnnotation> type, bool isConst, const std::map<std::string, Variable>& fields)
  : name(name), type(type), isConst(isConst), fields(fields) {}

ff::Compiler::Variable ff::Compiler::Variable::fromObject(const std::string& name, Ref<Object> object) {
  Variable var;
  var.name = name;
  var.isConst = true;
  var.type = TypeAnnotation::any();
  if (object->isInstance()) {
    if (object.as<Instance>()->getType()->equals(NativeFunctionType::getInstance().asRefTo<Object>())) {
      std::vector<Ref<TypeAnnotation>> argTypes;
      std::transform(
        BEGIN_END(object.as<NativeFunction>()->getArgs()),
        std::back_inserter(argTypes),
        [](const auto& arg) {
          return arg.type;
        }
      );

      var.type = FunctionAnnotation::create(argTypes, object.as<NativeFunction>()->getReturnType()).asRefTo<TypeAnnotation>();
    } else {
      var.type = TypeAnnotation::create(object.as<Instance>()->getType()->getTypeName());
    }
    for (auto field : object->getFields()) {
      var.fields[field.first] = Variable::fromObject(field.first, field.second);
    }
  } else if (object->isType()) {
    var.type = TypeAnnotation::create("type");
    for (auto field : object->getFields()) {
      var.fields[field.first] = Variable::fromObject(field.first, field.second);
    }
  } else {
    // throw CompieError // TODO:
  }
  return var;
}

void ff::Compiler::printScopes() {
  std::string prefix;
  for (int i = 0; i < m_scopes.size(); i++) {
    printScope(i, prefix);
    prefix += "  ";
  }
}

void ff::Compiler::printScope(int i, const std::string& prefix) {
  if (i == -1) {
    i = m_scopes.size() - 1;
  }
  for (auto& var : m_scopes[i].localVariables) {
    printf("%s%s: %s\n", prefix.c_str(), var.name.c_str(), var.type->toString().c_str());
  }
}

void ff::Compiler::printGlobals() {
  std::function<void(std::string, Variable&)> printGlobal = [&printGlobal](const std::string& prefix, Variable& var) {
    // printf("%s%s: %s", prefix.c_str(), var.name.c_str(), var.type->toString().c_str()); // FIXME: can fail if var.type is nullptr
    printf("%s%s: ", prefix.c_str(), var.name.c_str());
    printf("%s", var.type->toString().c_str());
    if (var.fields.size()) {
      printf(" {\n");
      for (auto& var : var.fields) {
        printGlobal(prefix + "  ", var.second);
        printf("\n");
      }
      printf("%s}", prefix.c_str());
    }
  };

  for (auto& var : m_globalVariables) {
    printGlobal("", var.second);
    printf("\n");
  }
}

ff::CompileError::CompileError(const std::string& filename, int line, const std::string& msg) : m_line(line), m_filename(filename), m_message(msg) {}

ff::CompileError::CompileError(const std::string& filename, int line, const char* fmt, ...) : m_line(line), m_filename(filename) {
  char* buffer = new char[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, 1024, fmt, args);
  va_end(args);
  m_message = buffer;
  delete [] buffer;
}

ff::CompileError& ff::CompileError::addNote(const std::string& note) {
  m_note = note;
  return *this;
}

ff::CompileError& ff::CompileError::addNote(const char* fmt, ...) {
  char* buffer = new char[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, 1024, fmt, args);
  va_end(args);
  m_note = buffer;
  delete [] buffer;
  return *this;
}

const char* ff::CompileError::what() const noexcept {
  return m_message.c_str();
}

void ff::CompileError::print() const {
  if (m_line == -1) {
    printf("%s: %sCompileError%s: %s\n", m_filename.c_str(), mrt::console::RED, mrt::console::RESET, m_message.c_str());
  } else {
    printf("%s:%d: %sCompileError%s: %s\n", m_filename.c_str(), m_line, mrt::console::RED, mrt::console::RESET, m_message.c_str());
  }
  if (!m_note.empty()) {
    info(m_note);
  }
}

ff::Compiler::Compiler() {
  // NOTE: Initialize info about built-in types
  m_globalVariables["int"] = Variable::fromObject("int", IntType::getInstance().asRefTo<Object>());
  m_globalVariables["float"] = Variable::fromObject("float", FloatType::getInstance().asRefTo<Object>());
  m_globalVariables["bool"] = Variable::fromObject("bool", BoolType::getInstance().asRefTo<Object>());
  m_globalVariables["string"] = Variable::fromObject("string", StringType::getInstance().asRefTo<Object>());
  m_globalVariables["dict"] = Variable::fromObject("dict", DictType::getInstance().asRefTo<Object>());
  m_globalVariables["vector"] = Variable::fromObject("vector", VectorType::getInstance().asRefTo<Object>());

  m_annotations["print"] = annotations::print;
}

ff::Ref<ff::Code> ff::Compiler::compile(const std::string& filename, ast::Node* node) {
  if (!node) {
    throw CompileError(filename, -1, "Unexpected Null Pointer");
  }

  if (node->getType() != ast::NTYPE_BLOCK) {
    throw CompileError(filename, -1, "Expected block");
  }

  m_filename = filename;

  evalNode(node);

#ifdef _FF_DEBUG_GLOBALS
  if (config::get("debug") != "0") {
    printGlobals();
  }
#endif

  return m_scopes.back().code;
}

ff::Ref<ff::Code>& ff::Compiler::getCode() {
  return m_scopes.back().code;
}

std::vector<ff::Compiler::Variable>& ff::Compiler::getLocals() {
  return m_scopes.back().localVariables;
}

bool ff::Compiler::localExists(const std::string& name) {
  return findLocal(name) != getLocals().end();
}

std::vector<ff::Compiler::Variable>::iterator ff::Compiler::findLocal(const std::string& name) {
  return std::find_if(getLocals().begin(), getLocals().end(), [&name](auto var) {
    return var.name == name;
  });
}

void ff::Compiler::setParentModule(const std::string& parentModule) {
  m_parentModuleName = parentModule;
}

void ff::Compiler::setThisModule(const std::string& thisModule) {
  m_thisModuleName = thisModule;
}

std::map<std::string, ff::Compiler::Variable>& ff::Compiler::getGlobals() {
  return m_globalVariables;
}

std::vector<std::string>& ff::Compiler::getImports() {
  return m_imports;
}

std::map<std::string, ff::Ref<mrt::DynamicLibrary>> ff::Compiler::getSharedLibs() {
  return std::move(m_sharedLibs);
}

void ff::Compiler::beginScope() {
  m_scopes.push_back({SCOPE_BLOCK, memory::construct<Code>(m_filename)});
#ifdef _FF_DEBUG_SCOPES
  if (config::get("debug") != "0") {
    std::string printPrefix = str::repeat("  ", m_scopes.size());
    printf("%sscope begin\n", printPrefix.c_str());
  }
#endif
}

void ff::Compiler::beginFunctionScope(ff::Ref<ff::TypeAnnotation> returnType) {
  m_scopes.push_back({
    SCOPE_FUNCTION,
    memory::construct<Code>(m_filename),
    {},
    0,
    true,
    returnType
  });
#ifdef _FF_DEBUG_SCOPES
  if (config::get("debug") != "0") {
    std::string printPrefix = str::repeat("  ", m_scopes.size());
    printf("%sfunction scope begin (ret=%s)\n", printPrefix.c_str(), returnType->toString().c_str());
  }
#endif
}

ff::Compiler::Scope ff::Compiler::endScope() {
#ifdef _FF_DEBUG_SCOPES
  if (config::get("debug") != "0") {
    std::string printPrefix = str::repeat("  ", m_scopes.size());
    printScope(-1, printPrefix + "  ");
    printf("%sscope end\n", printPrefix.c_str());
  }
#endif
  if (!m_scopes.back().localVariables.empty() && m_scopes.back().type != SCOPE_FUNCTION) {
    for (auto& var : getLocals()) {
      getCode()->push<uint8_t>(OP_POP);
    }
  }
  Scope scope = m_scopes.back();
  m_scopes.pop_back();
  return scope;
}

void ff::Compiler::beginBlock() {
  if (m_scopes.size() == 0) {
    m_scopes.push_back({SCOPE_BLOCK, memory::construct<Code>(m_filename)});
  } else {
    Ref<Code> code = m_scopes.back().code;
    m_scopes.push_back({SCOPE_BLOCK, code});
  }
#ifdef _FF_DEBUG_SCOPES
  if (config::get("debug") != "0") {
    std::string printPrefix = str::repeat("  ", m_scopes.size());
    printf("%sblock begin\n", printPrefix.c_str());
  }
#endif
}

void ff::Compiler::endBlock() {
#ifdef _FF_DEBUG_SCOPES
  if (config::get("debug") != "0") {
    std::string printPrefix = str::repeat("  ", m_scopes.size());
    printScope(-1, printPrefix + "  ");
    printf("%sblock end\n", printPrefix.c_str());
  }
#endif
  for (auto& var : m_scopes.back().localVariables) {
    getCode()->push<uint8_t>(OP_POP);
  }
  if (m_scopes.size() > 1) {
    m_scopes.pop_back();
  }
}

void ff::Compiler::beginLoop() {
  m_loops.push_back({});
}

void ff::Compiler::endLoop() {
  m_loops.pop_back();
}

ff::Compiler::LoopRecord& ff::Compiler::getLoop() {
  if (m_loops.empty()) {
    throw CompileError(m_filename, -1, "No loop records");
  }

  return m_loops.back();
}

bool ff::Compiler::isTopScope() const {
  return m_scopes.size() == 1;
}

void ff::Compiler::emitConstant(Ref<Object> obj) {
  unsigned constant = getCode()->addConstant(obj);
  getCode()->push<uint8_t>(OP_LOAD_CONSTANT);
  getCode()->push<uint32_t>(constant);
}

void ff::Compiler::emitCall(const std::string& callee) {
  resolveVariable(callee);
  getCode()->push<uint8_t>(OP_CALL);
}

uint16_t ff::Compiler::emitJump(Opcode op) {
  getCode()->pushInstruction(op);
  getCode()->push<uint16_t>(0xffff);
  return getCode()->size() - 2;
}

void ff::Compiler::patchJump(int offset) {
  int jump = getCode()->size() - offset - 2;
  abi::N32 data;
  data.u16[0] = jump;
  (*getCode())[offset] = data.u8[0];
  (*getCode())[offset+1] = data.u8[1];
}

void ff::Compiler::patchRemoteJump(int offset, int jump) {
  abi::N32 data;
  data.u16[0] = jump;
  (*getCode())[offset] = data.u8[0];
  (*getCode())[offset+1] = data.u8[1];
}

void ff::Compiler::emitLoop(int loopStart) {
  getCode()->pushInstruction(OP_LOOP);
  int offset = getCode()->size() - loopStart + 2;
  if (offset > UINT16_MAX) {
    throw CompileError(m_filename, -1, "Loop is too big");
  }
  getCode()->push<uint16_t>(offset);
}

ff::Ref<ff::TypeAnnotation> ff::Compiler::resolveVariable(const std::string& name, Opcode local, Opcode global, bool checkIsConst) {
  int localsSize = 0;
  for (int i = m_scopes.size() - 1; i > 0; i--) {
    localsSize += m_scopes[i].localVariables.size();
    if (m_scopes[i].type == SCOPE_FUNCTION) break;
  }
  for (int i = m_scopes.size() - 1; i > 0; i--) {
    localsSize -= m_scopes[i].localVariables.size();
    auto itr = std::find_if(m_scopes[i].localVariables.begin(), m_scopes[i].localVariables.end(), [&name](auto var) {
      return var.name == name;
    });
    if (itr != m_scopes[i].localVariables.end()) {
      getCode()->push<uint8_t>(local);
      getCode()->push<uint32_t>(itr - m_scopes[i].localVariables.begin() + localsSize);
      if (checkIsConst && itr->isConst) {
        throw CompileError(m_filename, -1, "Cannot assign to const variable '%s'", itr->name.c_str());
      }
      return itr->type;
    }
  }
  auto itr = m_globalVariables.find(name);
  if (itr != m_globalVariables.end()) {
    emitConstant(String::createInstance(name).asRefTo<Object>());
    getCode()->push<uint8_t>(global);
    if (checkIsConst && itr->second.isConst) {
      throw CompileError(m_filename, -1, "Cannot assign to const variable '%s'", itr->second.name.c_str());
    }
    return itr->second.type;
  } else {
    throw CompileError(m_filename, -1, "Unknown variable '%s'", name.c_str());
  }
  return TypeAnnotation::any();
}

ff::Ref<ff::TypeAnnotation> ff::Compiler::getVariableType(const std::string& name) {
  int localsSize = 0;
  for (int i = m_scopes.size() - 1; i > 0; i--) {
    localsSize += m_scopes[i].localVariables.size();
    if (m_scopes[i].type == SCOPE_FUNCTION) break;
  }
  for (int i = m_scopes.size() - 1; i > 0; i--) {
    localsSize -= m_scopes[i].localVariables.size();
    auto itr = std::find_if(m_scopes[i].localVariables.begin(), m_scopes[i].localVariables.end(), [&name](auto var) {
      return var.name == name;
    });
    if (itr != m_scopes[i].localVariables.end()) {
      return itr->type;
    }
  }
  auto itr = m_globalVariables.find(name);
  if (itr != m_globalVariables.end()) {
    return itr->second.type;
  } else {
    throw CompileError(m_filename, -1, "Unknown variable '%s'", name.c_str());
  }
  return TypeAnnotation::any();
}

ff::Ref<ff::TypeAnnotation> ff::Compiler::defineLocal(Variable var, int line, ast::Node* value, bool copyValue) {
  if (localExists(var.name)) {
    throw CompileError(m_filename, line, "Redeclaration of local variable");
  }
  if (value) {
    auto type = evalNode(value, copyValue);
    if (*type == *TypeAnnotation::nothing()) {
      throw CompileError(m_filename, line, "Value of type 'nothing' is invalid");
    }

    if (*type == *TypeAnnotation::any() && *var.type == *TypeAnnotation::any() && !var.type->isInferred && !type->isInferred) {
      warning("Couldn't infer value type for '%s', assuming 'any'", var.name.c_str());
      info("Add explicit 'any' type annotation to silence the warning");
    }

    if (*type != *TypeAnnotation::any()) {
      if (*var.type != *TypeAnnotation::any() && *var.type != *type) {
        throw CompileError(m_filename, -1,
          "TypeMismatch during variable definition (annotated type: %s, actual type: %s)",
          var.type->toString().c_str(), type->toString().c_str());
      }
      var.type = type;
    }
  } else {
    getCode()->push<uint8_t>(OP_NULL);
  }
  // var.isIitialized = true;
  getLocals().push_back(var);
  return var.type;
}

ff::Compiler::TypeInfo ff::Compiler::resolveCurrentModule() {
  if (m_modules.empty()) return {TypeAnnotation::any(), nullptr};
  std::string rootModuleName = m_modules.front();
  std::vector<TypeInfo> typeInfo;

  auto itr = m_globalVariables.find(rootModuleName);
  if (itr != m_globalVariables.end()) {
    emitConstant(String::createInstance(rootModuleName).asRefTo<Object>());
    getCode()->push<uint8_t>(OP_GET_GLOBAL);

    typeInfo.push_back({itr->second.type, &itr->second});

    for (int i = 1; i < m_modules.size(); i++) {
      auto fitr = typeInfo.back().var->fields.find(m_modules[i]);
      if (fitr != typeInfo.back().var->fields.end()) {
        emitConstant(String::createInstance(fitr->second.name).asRefTo<Object>());
        getCode()->pushInstruction(OP_GET_FIELD);

        typeInfo.push_back({fitr->second.type, &fitr->second});
      } else {
        throw CompileError(m_filename, -1, "Unknown field '%s'", m_modules[i].c_str());
      }
    }

  } else {
    throw CompileError(m_filename, -1, "Unknown variable '%s'", rootModuleName.c_str());
  }

  return typeInfo.back();
}

std::vector<ff::Function::Argument> ff::Compiler::parseArgs(ast::VarDeclList* args) {
  std::vector<Function::Argument> result;
  if (args) {
    std::transform(
      BEGIN_END(args->getList()),
      std::back_inserter(result),
      [](auto& varDecl) -> Function::Argument {
        return {varDecl->getName().str, varDecl->getVarType()};
      }
    );
  }
  return result;
}

void ff::Compiler::defineArgs(ast::VarDeclList* args) {
  if (args) {
    for (auto& varDecl : args->getList()) {
      Variable var(
        varDecl->getName().str,
        varDecl->getVarType(),
        varDecl->getConst(),
        {}
      );
      if (localExists(var.name)) {
        throw CompileError(m_filename, varDecl->getName().line, "Redeclaration of local variable in argument list");
      }
      getLocals().push_back(var);
    }
  }
}

ff::Compiler::TypeInfo ff::Compiler::evalSequenceElement(TypeInfo prev, ast::Node* node, bool& isFunction) {
  isFunction = false;
  if (node->getType() == ast::NTYPE_IDENTIFIER) {
    std::string name = node->as<ast::Identifier>()->getValue();
    emitConstant(String::createInstance(name).asRefTo<Object>());
    getCode()->pushInstruction(OP_GET_FIELD);
    Variable* var = nullptr;
    auto type = TypeAnnotation::any();
    if (prev.var) {
      if (prev.var->fields.find(name) != prev.var->fields.end()) {
        var = &prev.var->fields[name];
        type = var->type;
      }
    }
    return {type, var};
  } else if (node->getType() == ast::NTYPE_CALL) {
    bool explicitSelf = false;
    if (prev.var) {
      if (*prev.type == *TypeAnnotation::create("module")) {
        explicitSelf = true;
      } else {
        explicitSelf = prev.type->toString() == prev.var->name;
      }
    }
    isFunction = true;
    return {call(node, false, prev, explicitSelf), nullptr};
  } else {
    throw CompileError(m_filename, -1, "Expected filed or call");
  }
  return {TypeAnnotation::any(), nullptr};
}

ff::Compiler::TypeInfo ff::Compiler::evalSequenceStart(ast::Node* node) {
  if (node->getType() == ast::NTYPE_IDENTIFIER) {
    std::string name = node->as<ast::Identifier>()->getValue();
    auto type = resolveVariable(name);
    if (m_globalVariables.find(name) != m_globalVariables.end()) {
      return {type, &m_globalVariables[name]};
    }
    return {type, nullptr};
  } else if (node->getType() == ast::NTYPE_CALL) { // Call
    auto type = call(node, true);
    return {type, nullptr};
  } else {
    throw CompileError(m_filename, -1, "Expected identifier or call");
  }
  return {TypeAnnotation::any(), nullptr};
}

ff::Ref<ff::TypeAnnotation> ff::Compiler::identifier(ast::Node* node, bool copyValue) {
  ast::Identifier* ident = node->as<ast::Identifier>();
  auto type = resolveVariable(ident->getValue());
  if (type->annotationType != TypeAnnotation::TATYPE_FUNCTION) {
    if (copyValue) {
      getCode()->pushInstruction(OP_COPY);
    }
  }
  return type;
}

ff::Ref<ff::TypeAnnotation> ff::Compiler::sequence(ast::Node* node, bool copyValue) {
  auto seq = node->as<ast::Sequence>()->getSequence();
  TypeInfo typeInfo = evalSequenceStart(seq.front());
  bool isFunction = false;
  for (int i = 1; i < seq.size(); i++) {
    typeInfo = evalSequenceElement(typeInfo, seq[i], isFunction);
  }
  if (!isFunction && copyValue) {
    getCode()->pushInstruction(OP_COPY);
  }
  return typeInfo.type;
}

ff::Ref<ff::TypeAnnotation> ff::Compiler::binaryExpr(ast::Node* node) {
  ast::Binary* binary = node->as<ast::Binary>();
  auto leftType = evalNode(binary->getLeft(), false);
  auto rightType = evalNode(binary->getRight(), false);
  // TODO: Infer type from globals[leftType]->fields[__add__]->returnType, if impossible - return leftType
  switch (binary->getOperator().type) {
    case TOKEN_PLUS: {
      getCode()->pushInstruction(OP_ADD);
      return leftType;
    }
    case TOKEN_MINUS: {
      getCode()->pushInstruction(OP_SUB);
      return leftType;
    }
    case TOKEN_STAR: {
      getCode()->pushInstruction(OP_MUL);
      return leftType;
    }
    case TOKEN_SLASH: {
      getCode()->pushInstruction(OP_DIV);
      return leftType;
    }
    case TOKEN_EQUAL_EQUAL: {
      getCode()->pushInstruction(OP_EQ);
      return TypeAnnotation::create("bool", true);
    }
    case TOKEN_BANG_EQUAL: {
      getCode()->pushInstruction(OP_NEQ);
      return TypeAnnotation::create("bool", true);
    }
    case TOKEN_LESS: {
      getCode()->pushInstruction(OP_LT);
      return TypeAnnotation::create("bool", true);
    }
    case TOKEN_LESS_EQUAL: {
      getCode()->pushInstruction(OP_LE);
      return TypeAnnotation::create("bool", true);
    }
    case TOKEN_GREATER: {
      getCode()->pushInstruction(OP_GT);
      return TypeAnnotation::create("bool", true);
    }
    case TOKEN_GREATER_EQUAL: {
      getCode()->pushInstruction(OP_GE);
      return TypeAnnotation::create("bool", true);
    }
    case TOKEN_AND: {
      getCode()->pushInstruction(OP_AND);
      return TypeAnnotation::create("bool", true);
    }
    case TOKEN_OR: {
      getCode()->pushInstruction(OP_OR);
      return TypeAnnotation::create("bool", true);
    }
    default: {
      throw CompileError(m_filename, binary->getOperator().line, "Unknown binary operator '%s'", binary->getOperator().str.c_str());
    }
  }
  return TypeAnnotation::any();
}

ff::Ref<ff::TypeAnnotation> ff::Compiler::unaryExpr(ast::Node* node) {
  ast::Unary* unary = node->as<ast::Unary>();
  auto type = evalNode(unary->getValue(), false);
  switch (unary->getOperator().type) {
    case TOKEN_BANG: {
      getCode()->pushInstruction(OP_NOT);
      return TypeAnnotation::create("bool");
    }
    case TOKEN_MINUS: {
      getCode()->pushInstruction(OP_NEG);
      return type;
    }
    case TOKEN_INCREMENT: {
      getCode()->pushInstruction(OP_INC);
      return type;
    }
    case TOKEN_DECREMENT: {
      getCode()->pushInstruction(OP_DEC);
      return type;
    }
    default: {
      throw CompileError(m_filename, unary->getOperator().line, "Unknown unary operator '%s'", unary->getOperator().str.c_str());
    }
  }

  return TypeAnnotation::any();
}

ff::Ref<ff::TypeAnnotation> ff::Compiler::fndecl(ast::Node* node, bool isModule) {
  ast::Function* fn = node->as<ast::Function>();

  for (auto& annotation : node->getAnnotations()) {
    auto aitr = m_annotations.find(annotation);
    if (aitr != m_annotations.end()) {
      aitr->second(node);
    }
  }

  beginFunctionScope(fn->getFunctionType()->returnType);
  defineArgs(fn->getArgs());
  auto bodyType = evalNode(fn->getBody());
  Scope scope = endScope();

  if (*fn->getFunctionType()->returnType == *TypeAnnotation::any() && !fn->getFunctionType()->returnType->isInferred) {
    fn->getFunctionType()->isInferred = true;
    if (scope.returnStatements.empty()) {
      if (fn->getBody()->getType() == ast::NTYPE_BLOCK) {
        fn->getFunctionType()->returnType = TypeAnnotation::nothing();
      }
    } else {
      // NOTE: All types in `returnStatements` are guarranteed to be the same if return type is `any` and it's not inferred
      fn->getFunctionType()->returnType = scope.returnStatements.front();
    }
  }

  if (fn->getBody()->getType() != ast::NTYPE_BLOCK) {
    if (*fn->getFunctionType()->returnType != *TypeAnnotation::any()) {
      if (*fn->getFunctionType()->returnType != *bodyType) {
        throw CompileError(m_filename, -1,
          "TypeMismatch of function value (annotated type: %s, actual type: %s)",
          fn->getFunctionType()->returnType->toString().c_str(), bodyType->toString().c_str());
      }
    } else {
      if (!fn->getFunctionType()->isInferred) {
        fn->getFunctionType()->isInferred = true;
        fn->getFunctionType()->returnType = bodyType;
      }
    }
  }

  if (!isModule) {
    emitConstant(String::createInstance(fn->getName().str).asRefTo<Object>());
    getCode()->pushInstruction(OP_NEW_GLOBAL);
  }

  Variable var(
    fn->getName().str,
    fn->getFunctionType().asRefTo<TypeAnnotation>(),
    true,
    {}
  );
  m_globalVariables[var.name] = var;

  if (scope.code->size() == 0 || (*scope.code)[scope.code->size()-1] != OP_RETURN) {
    scope.code->pushInstruction(OP_RETURN);
  }

  Ref<Function> function = Function::createInstance(
    scope.code,
    parseArgs(fn->getArgs()),
    fn->getFunctionType().as<FunctionAnnotation>()->returnType
  );

  std::vector<Ref<Object>> annotations;
  std::transform(
    BEGIN_END(fn->getAnnotations()),
    std::back_inserter(annotations),
    [](const auto& annotation) {
      return String::createInstance(annotation).template asRefTo<Object>();
    }
  );
  function->setField("__annotations__", Vector::createInstance(annotations).asRefTo<Object>());

  if (isModule) {
    emitConstant(function.asRefTo<Object>());
    TypeInfo typeInfo = resolveCurrentModule();
    typeInfo.var->fields[var.name] = var;
    emitConstant(String::createInstance(fn->getName().str).asRefTo<Object>());
    getCode()->pushInstruction(OP_SET_FIELD);
  } else {
    emitConstant(function.asRefTo<Object>());
    emitConstant(String::createInstance(fn->getName().str).asRefTo<Object>());
    getCode()->pushInstruction(OP_SET_GLOBAL);
  }

  return fn->getFunctionType().asRefTo<TypeAnnotation>();
}

ff::Ref<ff::TypeAnnotation> ff::Compiler::vardecl(ast::Node* node, bool copyValue, bool isModule) {
  ast::VarDecl* varNode = node->as<ast::VarDecl>();

  for (auto& annotation : node->getAnnotations()) {
    auto aitr = m_annotations.find(annotation);
    if (aitr != m_annotations.end()) {
      aitr->second(node);
    }
  }

  Variable var {
    varNode->getName().str,
    varNode->getVarType(),
    varNode->getConst(),
    {}
  };
  if (isTopScope()) {
    if (isModule) {
      auto type = evalNode(varNode->getValue(), copyValue);
      TypeInfo typeInfo = resolveCurrentModule();
      typeInfo.var->fields[var.name] = var;
      
      if (*type == *TypeAnnotation::nothing()) {
        throw CompileError(m_filename, varNode->getName().line, "Value of type 'nothing' is invalid");
      }

      if (*type == *TypeAnnotation::any() && *varNode->getVarType() == *TypeAnnotation::any() && !varNode->getVarType()->isInferred && !type->isInferred) {
        warning("Couldn't infer value type for '%s', assuming 'any'", var.name.c_str());
        info("Add explicit 'any' type annotation to silence the warning");
      }

      if (type->toString() != "any") {
        if (*varNode->getVarType() != *TypeAnnotation::any() && *varNode->getVarType() != *type) {
          throw CompileError(m_filename, varNode->getName().line,
            "TypeMismatch during variable definition (annotated type: %s, actual type: %s)",
            varNode->getVarType()->toString().c_str(), type->toString().c_str());
        }
        typeInfo.var->fields[var.name].type = type;
      }

      emitConstant(String::createInstance(var.name).template asRefTo<Object>());
      getCode()->pushInstruction(OP_SET_FIELD);
      return typeInfo.var->fields[var.name].type;
    } else {
      if (m_globalVariables.find(var.name) != m_globalVariables.end()) {
        throw CompileError(m_filename, varNode->getName().line, "Redeclaration of global variable");
      }
      m_globalVariables[var.name] = var;
      emitConstant(String::createInstance(var.name).asRefTo<Object>());
      getCode()->pushInstruction(OP_NEW_GLOBAL);

      auto type = evalNode(varNode->getValue(), copyValue);
      if (*type == *TypeAnnotation::nothing()) {
        throw CompileError(m_filename, varNode->getName().line, "Value of type 'nothing' is invalid");
      }

      if (*type == *TypeAnnotation::any() && *varNode->getVarType() == *TypeAnnotation::any() && !varNode->getVarType()->isInferred && !type->isInferred) {
        warning("Couldn't infer value type for '%s', assuming 'any'", var.name.c_str());
        info("Add explicit 'any' type annotation to silence the warning");
      }

      if (type->toString() != "any") {
        if (*varNode->getVarType() != *TypeAnnotation::any() && *varNode->getVarType() != *type) {
          throw CompileError(m_filename, varNode->getName().line,
            "TypeMismatch during variable definition (annotated type: %s, actual type: %s)",
            varNode->getVarType()->toString().c_str(), type->toString().c_str());
        }
        // type.isInferred = true;
        m_globalVariables[var.name].type = type;
      }
      emitConstant(String::createInstance(var.name).asRefTo<Object>());
      getCode()->pushInstruction(OP_SET_GLOBAL);
    }
    return m_globalVariables[var.name].type;
  } else {
    return defineLocal(var, varNode->getName().line, varNode->getValue(), copyValue);
  }

  return TypeAnnotation::any();
}

ff::Ref<ff::TypeAnnotation> ff::Compiler::call(ast::Node* node, bool topLevelCallee, TypeInfo typeInfo, bool explicitSelf) {
  ast::Call* call = node->as<ast::Call>();

  if (call->getCallee()->getType() != ast::NTYPE_IDENTIFIER) {
    throw CompileError(m_filename, -1, "Callee is not an identifier");
  }

  std::string functionName = call->getCallee()->as<ast::Identifier>()->getValue();

  Ref<TypeAnnotation> type = TypeAnnotation::any();

  if (topLevelCallee) {
    type = getVariableType(functionName);
    if (type->annotationType != TypeAnnotation::TATYPE_FUNCTION) {
      warning("Type of `%s` is not callable ('%s')", functionName.c_str(), type->toString().c_str());
      type = TypeAnnotation::any();
    }
  } else {
    auto itr = m_globalVariables.find(typeInfo.type->toString());
    if (itr != m_globalVariables.end()) {
      auto fitr = itr->second.fields.find(functionName);
      if (fitr != itr->second.fields.end()) {
        if (fitr->second.type->annotationType == TypeAnnotation::TATYPE_FUNCTION) {
          type = fitr->second.type;
        } else {
          warning("Type of `%s` is not callable ('%s')", functionName.c_str(), fitr->second.type->toString().c_str());
        }
      }
    } else if (typeInfo.var) {
      auto fitr = typeInfo.var->fields.find(functionName);
      if (fitr != typeInfo.var->fields.end()) {
        if (fitr->second.type->annotationType == TypeAnnotation::TATYPE_FUNCTION) {
          type = fitr->second.type;
        } else {
          warning("Type of `%s` is not callable ('%s')", functionName.c_str(), fitr->second.type->toString().c_str());
        }
      }
    }
  }

  auto args = call->getArgs();

  for (int i = args.size() - 1; i >= 0; i--) {
    auto argType = evalNode(args[i]);
    if (type->annotationType == TypeAnnotation::TATYPE_FUNCTION) {
      Ref<TypeAnnotation> paramType;
      if (topLevelCallee) {
        paramType = type.asRefTo<FunctionAnnotation>()->arguments[i];
      } else {
        paramType = type.asRefTo<FunctionAnnotation>()->arguments[explicitSelf ? i : i + 1];
      }
      if (*paramType != *TypeAnnotation::any() && *argType != *paramType) {
        throw CompileError(m_filename, -1, "TypeMismatch: expected '%s', but got '%s' for argument %d of function '%s'",
          paramType->toString().c_str(),
          argType->toString().c_str(),
          i,
          functionName.c_str()
        );
      }
      if (paramType->isRef && !argType->isRef) {
        warning("Expected '%s' but got '%s' for argument %d of function '%s'",
          paramType->toString().c_str(),
          argType->toString().c_str(),
          i,
          functionName.c_str()
        );
        info("To silence the warning - pass a 'ref' or remove 'ref' from argument type annotation");
      }
    }
  }

  emitConstant(Int::createInstance(call->getArgs().size()).asRefTo<Object>());

  if (topLevelCallee) {
    evalNode(call->getCallee(), false);
    getCode()->pushInstruction(OP_CALL);
  } else {
    size_t nargs = call->getArgs().size();
    getCode()->pushInstruction(OP_PULL_UP);
    getCode()->push<uint16_t>(nargs + 2);
    emitConstant(String::createInstance(call->getCallee()->as<ast::Identifier>()->getValue()).asRefTo<Object>());
    getCode()->pushInstruction(OP_CALL_MEMBER);
  }

  if (!call->isReturnValueExpected()) {
    getCode()->pushInstruction(OP_POP);
  }

  if (type->annotationType == TypeAnnotation::TATYPE_FUNCTION) {
    return type.asRefTo<FunctionAnnotation>()->returnType;
  }
  return TypeAnnotation::any();
}

ff::Ref<ff::TypeAnnotation> ff::Compiler::lambda(ast::Node* node) {
  ast::Lambda* lambda = node->as<ast::Lambda>();

  beginFunctionScope(lambda->getFunctionType()->returnType);
  defineArgs(lambda->getArgs());
  auto bodyType = evalNode(lambda->getBody());
  Scope scope = endScope();

  if (*lambda->getFunctionType()->returnType == *TypeAnnotation::any() && !lambda->getFunctionType()->returnType->isInferred) {
    lambda->getFunctionType()->isInferred = true;
    if (scope.returnStatements.empty()) {
      if (lambda->getBody()->getType() == ast::NTYPE_BLOCK) {
        lambda->getFunctionType()->returnType = TypeAnnotation::nothing();
      }
    } else {
      // NOTE: All types in `returnStatements` are guarranteed to be the same if return type is `any` and it's not inferred
      lambda->getFunctionType()->returnType = scope.returnStatements.front();
    }
  }

  if (lambda->getBody()->getType() != ast::NTYPE_BLOCK) {
    if (*lambda->getFunctionType()->returnType != *TypeAnnotation::any()) {
      if (*lambda->getFunctionType()->returnType != *bodyType) {
        throw CompileError(m_filename, -1,
          "TypeMismatch of function value (annotated type: %s, actual type: %s)",
          lambda->getFunctionType()->returnType->toString().c_str(), bodyType->toString().c_str());
      }
    } else {
      if (!lambda->getFunctionType()->isInferred) {
        lambda->getFunctionType()->isInferred = true;
        lambda->getFunctionType()->returnType = bodyType;
      }
    }
  }

  if (scope.code->size() == 0 || (*scope.code)[scope.code->size()-1] != OP_RETURN) {
    scope.code->pushInstruction(OP_RETURN);
  }

  emitConstant(Function::createInstance(
    scope.code,
    parseArgs(lambda->getArgs()),
    lambda->getFunctionType().as<FunctionAnnotation>()->returnType
  ).asRefTo<Object>());

  return lambda->getFunctionType().asRefTo<TypeAnnotation>();
}

ff::Ref<ff::TypeAnnotation> ff::Compiler::assignment(ast::Node* node, bool copyValue) {
  // TODO: Check fields type
  ast::Assignment* ass = node->as<ast::Assignment>();
  auto valueType = evalNode(ass->getValue(), copyValue);
  if (ass->getAssignee()->getType() == ast::NTYPE_SEQUENCE) {
    auto seq = ass->getAssignee()->as<ast::Sequence>()->getSequence();

    evalNode(seq.front(), false);

    if (seq.size() > 2) {
      for (int i = 1; i < seq.size()-3; i++) {
        if (seq[i]->getType() == ast::NTYPE_IDENTIFIER) {
          emitConstant(String::createInstance(seq[i]->as<ast::Identifier>()->getValue()).asRefTo<Object>());
        } else if (seq[i]->getType() == ast::NTYPE_CALL) { // is this even legal?
          call(seq[i], false);
        }
      }
    }
    if (seq.back()->getType() != ast::NTYPE_IDENTIFIER) {
      throw CompileError(m_filename, -1, "Cannot set anything other than a field");
    }
    emitConstant(String::createInstance(seq.back()->as<ast::Identifier>()->getValue()).asRefTo<Object>());
    getCode()->pushInstruction(ass->getIsRefAssignment() ? OP_SET_FIELD_REF : OP_SET_FIELD);
  } else if (ass->getAssignee()->getType() == ast::NTYPE_IDENTIFIER) {
    auto variableType = resolveVariable(
      ass->getAssignee()->as<ast::Identifier>()->getValue(),
      ass->getIsRefAssignment() ? OP_SET_LOCAL_REF  : OP_SET_LOCAL,
      ass->getIsRefAssignment() ? OP_SET_GLOBAL_REF : OP_SET_GLOBAL,
      true // Check if variable is const
    );
    if (*variableType != *TypeAnnotation::any() && *variableType != *valueType) {
      throw CompileError(m_filename, -1,
        "TypeMismatch during assignment (annotated type: %s, value type: %s)",
        variableType->toString().c_str(), valueType->toString().c_str());
    }
  } else {
    throw CompileError(m_filename, -1, "Undefined variable '%s'", ass->getAssignee()->toString().c_str());
  }
  return valueType;
}

ff::Ref<ff::TypeAnnotation> ff::Compiler::cast(ast::Node* node, bool copyValue) {
  ast::Cast* cast = node->as<ast::Cast>();
  evalNode(cast->getValue());
  // NOTE: cast type can be anything (i.e. int | float | (int) -> int), so it must be checked that we recieved a concrete type
  if (cast->getCastType()->annotationType != TypeAnnotation::TATYPE_DEFAULT) {
    throw CompileError(m_filename, -1, "Invalid type for cast '%s'", cast->getCastType()->toString().c_str());
  }
  // NOTE: If cast type is `any` the value is returned as-is and it's type is assumed by the compiler to be `any`
  if (copyValue) {
    getCode()->pushInstruction(OP_COPY);
  }
  if (*cast->getCastType() != *TypeAnnotation::any()) {
    emitConstant(String::createInstance(cast->getCastType()->toString()).asRefTo<Object>());
    getCode()->pushInstruction(OP_CAST);
  }
  return cast->getCastType();
}

ff::Ref<ff::TypeAnnotation> ff::Compiler::ref(ast::Node* node) {
  ast::Ref* ref = node->as<ast::Ref>();
  auto type = evalNode(ref->getValue(), false);
  type = type->copy();
  type->isRef = true;
  return type;
}

void ff::Compiler::returnCall(ast::Node* node) {
  auto type = evalNode(node->as<ast::Return>()->getValue());

  int i = m_scopes.size() - 1;
  while (!m_scopes[i].isFunctionScope && i > 0) {
    i--;
  }
  if (i == 0 && !m_scopes[i].isFunctionScope) {
    throw CompileError(m_filename, -1, "return: No surrounding function scope can be found");
  }
  m_scopes[i].returnStatements.push_back(type);
  if (m_scopes[i].returnType.get()) {
    if (*m_scopes[i].returnType != *TypeAnnotation::any()) {
      if (*m_scopes[i].returnType != *type) {
        throw CompileError(m_filename, -1,
          "TypeMismatch of return type (expected type: %s, actual type: %s)",
          m_scopes[i].returnType->toString().c_str(), type->toString().c_str());
      }
    } else {
      if (!m_scopes[i].returnType->isInferred) {
        for (auto& t : m_scopes[i].returnStatements) {
          if (*t != *type) { // cppcheck-suppress useStlAlgorithm
            throw CompileError(m_filename, -1,
              "TypeMismatch: return type conflicts with previously returned value (previous return type: %s, actual type: %s)",
              t->toString().c_str(), type->toString().c_str()
            ).addNote("Add return type annotation to fix the error (any or union of possible return types)");
          }
        }
      }
    }
  }
  getCode()->pushInstruction(OP_RETURN);
}

ff::Ref<ff::TypeAnnotation> ff::Compiler::dict(ast::Node* node) {
  ast::Dict* dict = node->as<ast::Dict>();

  emitConstant(Dict::createInstance({}).asRefTo<Object>());

  for (auto& p : dict->getFields()) {
    getCode()->pushInstruction(OP_DUP); // object
    auto type = evalNode(p.second); // value
    bool isRef = p.second->getType() == ast::NTYPE_REF;

    getCode()->pushInstruction(OP_PULL_UP); // OP_SET_FIELD expects [ field, object, value ]
    getCode()->push<uint16_t>(2);

    emitConstant(String::createInstance(p.first).asRefTo<Object>()); // field
    getCode()->pushInstruction(isRef ? OP_SET_FIELD_REF : OP_SET_FIELD);
  }

  return TypeAnnotation::create("dict");
}

ff::Ref<ff::TypeAnnotation> ff::Compiler::vector(ast::Node* node) {
  ast::Vector* vec = node->as<ast::Vector>();

  emitConstant(Vector::createInstance({}).asRefTo<Object>());

  auto type = TypeAnnotation::create("vector");

  for (auto& e : vec->getElements()) {
    getCode()->pushInstruction(OP_DUP);
    ast::Node* call_ = new ast::Call(new ast::Identifier({TOKEN_IDENTIFIER, "append", -1}), {e}, false);
    call(call_, false, {type, nullptr});
  }

  return type;
}

void ff::Compiler::block(ast::Node* node) {
  beginBlock();
  for (auto bodyNode : node->as<ast::Block>()->getBody()) {
    evalNode(bodyNode);
  }
  endBlock();
}

void ff::Compiler::ifstmt(ast::Node* node) {
  ast::If* if_ = node->as<ast::If>();
  evalNode(if_->getCondition());
  uint16_t elseJump = emitJump(OP_JUMP_FALSE);
  evalNode(if_->getBody());
  uint16_t endJump = emitJump(OP_JUMP);
  patchJump(elseJump);
  if (if_->getElseBody()) {
    evalNode(if_->getElseBody());
  }
  patchJump(endJump);
}

void ff::Compiler::loopstmt(ast::Node* node) {
  ast::Loop* loop_ = node->as<ast::Loop>();
  uint16_t loopStart = getCode()->size();
  beginLoop();
  evalNode(loop_->getBody());

  emitLoop(loopStart);

  for (int continue_jump : getLoop().continue_jumps) {
    /* NOTE: Patch an `OP_LOOP`
       continue_jump - point from where he jump is made
       loopStart - destination
       since `OP_LOOP OFFSET` affecively does `ip -= OFFSET`,
       we calculate offset from jump origin to jump destination */
    patchRemoteJump(continue_jump, continue_jump - loopStart + 2);
  }

  for (int break_jump : getLoop().break_jumps) {
    patchJump(break_jump);
  }

  endLoop();
}

void ff::Compiler::whilestmt(ast::Node* node) {
  ast::While* while_ = node->as<ast::While>();
  uint16_t loopStart = getCode()->size();

  evalNode(while_->getCondition());
  uint16_t condition_jump = emitJump(OP_JUMP_FALSE);

  beginLoop();
  evalNode(while_->getBody());
  emitLoop(loopStart);
  patchJump(condition_jump);

  for (int continue_jump : getLoop().continue_jumps) {
    // NOTE: See note in loopstmt for explanation
    patchRemoteJump(continue_jump, continue_jump - loopStart + 2);
  }

  for (int break_jump : getLoop().break_jumps) {
    patchJump(break_jump);
  }

  endLoop();
}

void ff::Compiler::forstmt(ast::Node* node) {
  ast::For* for_ = node->as<ast::For>();

  beginBlock();

  evalNode(for_->getInit());

  int loopStart = getCode()->size();

  int loopExit = -1;
  if (for_->getCondition()) {
    evalNode(for_->getCondition());
    loopExit = emitJump(OP_JUMP_FALSE);
  }

  if (for_->getIncrement()) {
    int bodyJump = emitJump(OP_JUMP);
    int incrementStart = getCode()->size();

    evalNode(for_->getIncrement());
    getCode()->pushInstruction(OP_POP);

    emitLoop(loopStart);
    loopStart = incrementStart;
    patchJump(bodyJump);
  }

  beginLoop();
  evalNode(for_->getBody());

  emitLoop(loopStart);

  if (loopExit != -1) {
    patchJump(loopExit);
  }

  for (int continue_jump : getLoop().continue_jumps) {
    // NOTE: See note in loopstmt for explanation
    patchRemoteJump(continue_jump, continue_jump - loopStart + 2);
  }

  for (int break_jump : getLoop().break_jumps) {
    patchJump(break_jump);
  }

  endLoop();
  endBlock();
}

void ff::Compiler::import(ast::Node* node, bool isModule) {
  ast::Import* imp = node->as<ast::Import>();

  std::string cwd = path::getcwd();
  std::string filedir;
  if (path::isRoot(m_filename)) {
    filedir = path::getFolder(m_filename);
  } else {
    filedir = path::concat(cwd, path::getFolder(m_filename));
  }
  std::vector<std::string> importPaths = {filedir, cwd};
  const char* envImportPath = std::getenv(FF_IMPORT_PATH_ENV_VAR);
  if (envImportPath) {
    auto envImports = mrt::str::split(envImportPath, ":");
    importPaths.insert(importPaths.end(), envImports.begin(), envImports.end());
  }

  for (auto& import : imp->getImports()) {
    std::string name = path::stripExtension(path::getFile(import));
    if (std::find(m_imports.begin(), m_imports.end(), name) != m_imports.end()) {
      warning("Module '%s' has already been imported, skipping", name.c_str());
      continue;
    }
    if (name == m_parentModuleName) {
      throw CompileError(m_filename, -1, "Circular import detected (module '%s' from module '%s')", name.c_str(), m_thisModuleName.empty() ? "<no name>" : m_thisModuleName.c_str());
    }

    ModuleInfo modInfo;
    if (mrt::str::endsWith(import, ".ffmod") || mrt::str::endsWith(import, ".so")) {
      modInfo = loadNativeModule(name, config::format(path::getImportFileFromPath(import, importPaths)));
    } else {
      modInfo = loadModule(name, config::format(path::getImportFileFromPath(import, importPaths)), m_thisModuleName);
    }

    getCode()->addModule(name, modInfo.module.asRefTo<Object>());
    m_globalVariables[name] = modInfo.var;
    m_imports.push_back(name);

    for (auto& module : modInfo.imports) {
      if (std::find(m_imports.begin(), m_imports.end(), module.name) != m_imports.end()) {
        continue;
      }
      getCode()->addModule(module.name, module.module.asRefTo<Object>());
      m_globalVariables[module.name] = module.var;
      m_imports.push_back(module.name);
    }

    for (auto& dl : modInfo.sharedLibs) {
      m_sharedLibs[dl.first] = std::move(dl.second);
    }

    for (auto& annotation : modInfo.annotations) {
      if (m_annotations.find(annotation.first) != m_annotations.end()) {
        continue;
      }
      m_annotations[annotation.first] = annotation.second;
    }
  }
}

void ff::Compiler::module(ast::Node* node, bool isModule) {
  ast::Module* mod = node->as<ast::Module>();

  Variable var {
    mod->getName(),
    TypeAnnotation::create("module"),
    true,
    {}
  };

  if (m_globalVariables.find(var.name) != m_globalVariables.end()) {
    throw CompileError(m_filename, -1, "Redeclaration of global variable");
  }
  m_globalVariables[var.name] = var;

  if (isModule) {
    emitConstant(Module::createInstance(var.name).asRefTo<Object>());
    TypeInfo typeInfo = resolveCurrentModule();
    if (!typeInfo.var) {
      throw CompileError(m_filename, -1, "Coudn't resolve current module");
    }
    typeInfo.var->fields[var.name] = var;
    emitConstant(String::createInstance(var.name).template asRefTo<Object>());
    getCode()->pushInstruction(OP_SET_FIELD);
  } else {
    emitConstant(String::createInstance(var.name).asRefTo<Object>());
    getCode()->pushInstruction(OP_NEW_GLOBAL);

    emitConstant(Module::createInstance(var.name).asRefTo<Object>());
    emitConstant(String::createInstance(var.name).asRefTo<Object>());
    getCode()->pushInstruction(OP_SET_GLOBAL);

  }

  m_modules.push_back(var.name);
  for (auto bodyNode : mod->getBody()->as<ast::Block>()->getBody()) {
    evalNode(bodyNode, true, true);
  }
  m_modules.pop_back();
}

ff::Ref<ff::TypeAnnotation> ff::Compiler::evalNode(ast::Node* node, bool copyValue, bool isModule) {
  if (!node) return TypeAnnotation::nothing();
#ifdef _FF_EVAL_NODE_DEBUG
  printf("evalNode: ptr=%p type=%s\n", node, ast::nodeTypeToString(node->getType()).c_str());
#endif
  switch (node->getType()) {
    case ast::NTYPE_FLOAT_LITERAL: {
      emitConstant(Float::createInstance(node->as<ast::FloatLiteral>()->getValue()).asRefTo<Object>());
      return TypeAnnotation::create("float");
    }
    case ast::NTYPE_INTEGER_LITERAL: {
      emitConstant(Int::createInstance(node->as<ast::IntegerLiteral>()->getValue()).asRefTo<Object>());
      return TypeAnnotation::create("int");
    }
    case ast::NTYPE_STRING_LITERAL: {
      emitConstant(String::createInstance(node->as<ast::StringLiteral>()->getValue()).asRefTo<Object>());
      return TypeAnnotation::create("string");
    }
    case ast::NTYPE_NULL: {
      getCode()->pushInstruction(OP_NULL);
      return TypeAnnotation::create("null");
    }
    case ast::NTYPE_GROUP_EXPR: {
      return evalNode(node->as<ast::Group>()->getValue(), copyValue);
    }
    case ast::NTYPE_UNARY_EXPR: {
      return unaryExpr(node);
    }
    case ast::NTYPE_BINARY_EXPR: {
      return binaryExpr(node);
    }
    case ast::NTYPE_IDENTIFIER: {
      return identifier(node, copyValue);
    }
    case ast::NTYPE_SEQUENCE: {
      return sequence(node, copyValue);
    }
    case ast::NTYPE_FUNCTION: {
      return fndecl(node, isModule);
    }
    case ast::NTYPE_VAR_DECL: {
      return vardecl(node, copyValue, isModule);
    }
    case ast::NTYPE_CALL: {
      return call(node, true);
    }
    case ast::NTYPE_ASSIGNMENT: {
      return assignment(node, copyValue);
    }
    case ast::NTYPE_LAMBDA: {
      return lambda(node);
    }
    case ast::NTYPE_RETURN: {
      returnCall(node);
      break;
    }
    case ast::NTYPE_BLOCK: {
      block(node);
      break;
    }
    case ast::NTYPE_IF: {
      ifstmt(node);
      break;
    }
    case ast::NTYPE_LOOP: {
      loopstmt(node);
      break;
    }
    case ast::NTYPE_WHILE: {
      whilestmt(node);
      break;
    }
    case ast::NTYPE_FOR: {
      forstmt(node);
      break;
    }
    case ast::NTYPE_CONTINUE: {
      getLoop().continue_jumps.push_back(emitJump(OP_LOOP));
      return TypeAnnotation::nothing();
    }
    case ast::NTYPE_BREAK: {
      getLoop().break_jumps.push_back(emitJump(OP_JUMP));
      return TypeAnnotation::nothing();
    }
    case ast::NTYPE_TRUE: {
      getCode()->pushInstruction(OP_TRUE);
      return TypeAnnotation::create("bool");
    }
    case ast::NTYPE_FALSE: {
      getCode()->pushInstruction(OP_FALSE);
      return TypeAnnotation::create("bool");
    }
    case ast::NTYPE_REF: {
      return ref(node);
    }
    case ast::NTYPE_FOREACH:
    case ast::NTYPE_EXPR_LIST_EXPR: {
      throw CompileError(m_filename, -1, "Unimplemented");
    }
    case ast::NTYPE_CAST_EXPR: {
      return cast(node, false);
    }
    case ast::NTYPE_DICT: {
      return dict(node);
    }
    case ast::NTYPE_VECTOR: {
      return vector(node);
    }
    case ast::NTYPE_IMPORT: {
      import(node, isModule);
      return TypeAnnotation::nothing();
    }
    case ast::NTYPE_MODULE: {
      module(node, isModule);
      return TypeAnnotation::nothing();
    }
    case ast::NTYPE_PRINT: {
      evalNode(node->as<ast::Print>()->getValue(), false);
      getCode()->pushInstruction(OP_PRINT);
      return TypeAnnotation::nothing();
    }
#ifdef _DEBUG
    case ast::NTYPE_BREAKPOINT: {
      getCode()->pushInstruction(OP_BREAKPOINT);
      return TypeAnnotation::nothing();
    }
#endif
    default: {
      throw CompileError(m_filename, -1, "Unknown AST node: type=%d", (int)node->getType());
      // throw CompileError(m_filename, -1, "Unknown AST node: type=%d str='%s'", (int)node->getType(), node->toString().c_str()); // FIXME: can fail if node in nullptr
      break;
    }
  }

  return TypeAnnotation::any();
}

ff::Ref<ff::Code> ff::compile(const std::string& src, const std::string& filename) {
  Scanner scanner(filename, src);
  auto tokens = scanner.tokenize();
  Parser parser(filename, tokens);
  auto tree = parser.parse();
  Compiler compiler;
  return compiler.compile(filename, tree);
}
