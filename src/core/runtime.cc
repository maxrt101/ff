#include <ff/runtime.h>
#include <ff/config.h>
#include <ff/types.h>
#include <ff/builtins.h>
#include <mrt/console/colors.h>

using namespace ff::types;

ff::RuntimeError::RuntimeError(const std::string& filename, int line, const std::string& msg) : m_filename(filename), m_line(line), m_message(msg) {}

ff::RuntimeError ff::RuntimeError::flcreate(const std::string& filename, int line, const std::string& msg) {
  return RuntimeError(filename, line, msg);
}

ff::RuntimeError ff::RuntimeError::flcreatef(const std::string& filename, int line, const char* fmt, ...) {
  char* buffer = new char[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, 1024, fmt, args);
  va_end(args);
  std::string message = buffer;
  delete [] buffer;
  return RuntimeError::flcreate(filename, line, message);
}

ff::RuntimeError ff::RuntimeError::flvcreatef(const std::string& filename, int line, const char* fmt, va_list args) {
  char* buffer = new char[1024];
  vsnprintf(buffer, 1024, fmt, args);
  std::string message = buffer;
  delete [] buffer;
  return RuntimeError::flcreate(filename, line, message);
}

ff::RuntimeError ff::RuntimeError::create(const std::string& msg) {
  return RuntimeError::flcreate("", -1, msg);
}

ff::RuntimeError ff::RuntimeError::createf(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  RuntimeError error = RuntimeError::flvcreatef("", -1, fmt, args);
  va_end(args);
  return error;
}

ff::RuntimeError ff::RuntimeError::vcreatef(const char* fmt, va_list args) {
  return RuntimeError::flvcreatef("", -1, fmt, args);
}

const char* ff::RuntimeError::what() const noexcept {
  return m_message.c_str();
}

void ff::RuntimeError::print() const {
  printf("%s%s%s%s%sRuntimeError%s: %s\n",
    m_filename.c_str(), (m_filename.empty() ? "" : ":"), 
    (m_line == -1 ? "" : std::to_string(m_line).c_str()),
    (m_line == -1 ? "" : ": "),
    mrt::console::RED, mrt::console::RESET,
    m_message.c_str());
}

ff::VM::VM() {
  m_globals["int"]    = IntType::getInstance().asRefTo<Object>();
  m_globals["bool"]   = BoolType::getInstance().asRefTo<Object>();
  m_globals["float"]  = FloatType::getInstance().asRefTo<Object>();
  m_globals["string"] = StringType::getInstance().asRefTo<Object>();
  m_globals["dict"]   = DictType::getInstance().asRefTo<Object>();
  m_globals["vector"] = VectorType::getInstance().asRefTo<Object>();
  m_globals["exit"]   = obj(fn_exit);
  m_globals["assert"] = obj(fn_assert);
  m_globals["type"] = obj(fn_type);
}

ff::VM::~VM() {}

void ff::VM::run(Ref<Code> code) {
  runCode(code);
}

void ff::VM::runMain(Ref<Code> code) {
  runCode(code);
  if (m_globals.find(config::get("entry")) == m_globals.end()) {
    throw createError("Cannot find entry function ('%s')", config::get("entry").c_str());
  }
  call(config::get("entry"));
}

void ff::VM::run() {
  m_running = true;
  getCode()->resetRead();
  while (m_running && !m_requestStop && getCode()->canRead()) {
    m_running = executeInstruction((Opcode)getCode()->read<uint8_t>());
  }
}

void ff::VM::stop() {
  m_requestStop = true;
}

ff::VM::CallFrame& ff::VM::currentFrame() {
  return m_callStack.peek();
}

int ff::VM::getReturnCode() {
  return m_returnCode;
}

void ff::VM::setReturnCode(int returnCode) {
  m_returnCode = returnCode;
}

ff::Stack<ff::VM::StackType>& ff::VM::getStack() {
  return currentFrame().context.stack;
}

std::map<std::string, ff::Ref<ff::Object>>& ff::VM::getGlobals() {
  return m_globals;
}

ff::Ref<ff::Code>& ff::VM::getCode() {
  return currentFrame().context.code;
}

void ff::VM::push(Ref<Object> obj) {
  getStack().push(obj);
}

ff::Ref<ff::Object> ff::VM::pop() {
  return getStack().pop();
}

std::vector<ff::Ref<ff::Object>> ff::VM::pop(int count, bool reverse) {
  std::vector<Ref<Object>> result;
  for (int i = 0; i < count; i++) {
    if (reverse) {
      result.insert(result.begin(), getStack().pop());
    } else {
      result.push_back(getStack().pop());
    }
  }
  return result;
}

std::vector<ff::Ref<ff::Object>> ff::VM::popFrom(int frameOffset, int count) {
  std::vector<Ref<Object>> result;
  for (int i = 0; i < count; i++) {
    result.insert(result.begin(), m_callStack.peek(frameOffset).context.stack.pop());
  }
  return result;
}

void ff::VM::jumpForward(uint32_t offset) {
  getCode()->setReadIndex(getCode()->getReadIndex() + offset);
}

void ff::VM::jump(uint32_t offset) {
  getCode()->setReadIndex(offset);
}

uint8_t ff::VM::current() {
  return (*getCode())[getCode()->getReadIndex()];
}

uint8_t ff::VM::peek(int i) {
  return (*getCode())[getCode()->getReadIndex() + i];
}

ff::RuntimeError ff::VM::createError(const std::string& msg) {
  return RuntimeError::flcreate(getCode()->getFilename(), getCode()->getLine(getCode()->getReadIndex()), msg);
}

ff::RuntimeError ff::VM::createError(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  auto err = RuntimeError::flvcreatef(getCode()->getFilename(), getCode()->getLine(getCode()->getReadIndex()), fmt, args);
  va_end(args);
  return err;
}

void ff::VM::call(Ref<Object> object, int argc) {
  if (!object.get()) {
    throw createError("cannot call null");
  }
  if (isOfType(object, FunctionType::getInstance())) {
    Ref<Function> fn = object.asRefTo<Function>();
    if (fn->args.size() != argc) {
      throw createError("Expected %d arguments, but got %d", fn->args.size(), argc);
    }
    callFunction(fn, pop(fn->args.size(), true));
  } else if (isOfType(object, NativeFunctionType::getInstance())) {
    Ref<NativeFunction> fn = object.asRefTo<NativeFunction>();
    if (fn->args.size() != argc) {
      throw createError("Expected %d arguments, but got %d", fn->args.size(), argc);
    }
    callNativeFunction(fn, pop(fn->args.size()));
  } else {
    throw createError("Attempt to call an object of type '%s'", object.as<Instance>()->getType()->getTypeName().c_str());
  }
}

void ff::VM::call(Ref<Object> object, const std::vector<Ref<Object>>& args) {
  if (isOfType(object, FunctionType::getInstance())) {
    callFunction(object.asRefTo<Function>(), args);
  } else if (isOfType(object, NativeFunctionType::getInstance())) {
    callNativeFunction(object.asRefTo<NativeFunction>(), args);
  } else {
    throw createError("Attempt to call an object of type '%s'", object.as<Instance>()->getType()->getTypeName().c_str());
  }
}

void ff::VM::callMember(Ref<Object> self, const std::string& memberName, int argc) {
  if (!self.get()) {
    throw createError("cannot call member of null");
  }
  bool implicitSelf = true;
  Ref<Object> fnObject;
  if (self->isInstance()) {
    if (isOfType(self, ModuleType::getInstance())) {
      implicitSelf = false;
    }
    if (self->hasField(memberName)) {
      fnObject = self->getField(memberName);
    } else if (self.as<Instance>()->getType()->hasField(memberName)) {
      fnObject = self.as<Instance>()->getType()->getField(memberName);
    } else {
      throw createError("Member '%s' cannot be found", memberName.c_str());
    }
  } else {
    implicitSelf = false;
    if (self->hasField(memberName)) {
      fnObject = self->getField(memberName);
    } else {
      throw createError("Member '%s' cannot be found", memberName.c_str());
    }
  }
  if (isOfType(fnObject, FunctionType::getInstance())) {
    Ref<Function> fn = fnObject.asRefTo<Function>();
    if (fn->args.size() - (implicitSelf ? 1 : 0) != argc) {
      throw createError("Expected %d arguments, but got %d", fn->args.size()-1, argc);
    }
    std::vector<Ref<Object>> args = pop(argc);
    if (implicitSelf) {
      args.insert(args.begin(), self);
    }
    callFunction(fn, args);
  } else if (isOfType(fnObject, NativeFunctionType::getInstance())) {
    Ref<NativeFunction> fn = fnObject.asRefTo<NativeFunction>();
    if (fn->args.size() - (implicitSelf ? 1 : 0) != argc) {
      throw createError("Expected %d arguments, but got %d", fn->args.size()-1, argc);
    }
    std::vector<Ref<Object>> args = pop(argc);
    if (implicitSelf) {
      args.insert(args.begin(), self);
    }
    callNativeFunction(fn, args);
  } else {
    throw createError("Attempt to call an object of type '%s'", fnObject.as<Instance>()->getType()->getTypeName().c_str());
  }
}

void ff::VM::callMember(Ref<Object> self, const std::string& memberName, const std::vector<Ref<Object>>& args) {
  if (!self.get()) {
    throw createError("cannot call member of null");
  }
  Ref<Object> fnObject;
  if (self->isInstance()) {
    if (self->hasField(memberName)) {
      fnObject = self->getField(memberName);
    } else if (self.as<Instance>()->getType()->hasField(memberName)) {
      fnObject = self.as<Instance>()->getType()->getField(memberName);
    } else {
      throw createError("Member '%s' cannot be found", memberName.c_str());
    }
  } else {
    if (self->hasField(memberName)) {
      fnObject = self->getField(memberName);
    } else {
      throw createError("Member '%s' cannot be found", memberName.c_str());
    }
  }
  if (isOfType(fnObject, FunctionType::getInstance())) {
    Ref<Function> fn = fnObject.asRefTo<Function>();
    if (fn->args.size() != args.size()) {
      throw createError("%s: Expected %d arguments, but got %d", memberName.c_str(), fn->args.size()-1, args.size());
    }
    callFunction(fn, args);
  } else if (isOfType(fnObject, NativeFunctionType::getInstance())) {
    Ref<NativeFunction> fn = fnObject.asRefTo<NativeFunction>();
    if (fn->args.size() != args.size()) {
      throw createError("%s: Expected %d arguments, but got %d", memberName.c_str(), fn->args.size()-1, args.size());
    }
    callNativeFunction(fn, args);
  } else {
    throw createError("Attempt to call an object of type '%s'", fnObject.as<Instance>()->getType()->getTypeName().c_str());
  }
}

void ff::VM::callFunction(Ref<Function> fn, const std::vector<Ref<Object>>& args) {
  if (config::get("debug") != "0") {
    printf("     | CALL %p\n", fn.get());
  }
  runCode(fn->code, args);
}

void ff::VM::callNativeFunction(Ref<NativeFunction> fn, const std::vector<Ref<Object>>& args) {
  if (config::get("debug") != "0") {
    printf("     | CALL_NATIVE %p\n", fn.get());
  }
  push(fn->func(this, args));
}

void ff::VM::runCode(Ref<Code> code, std::vector<Ref<Object>> args) {
  m_callStack.push({Stack<Ref<Object>>(), 0, code});
  for (auto& module : code->getModules()) {
    m_globals[module.first] = module.second;
  }
  for (auto itr = args.rbegin(); itr != args.rend(); itr++) {
    push(*itr);
  }
  run();
}

void ff::VM::call(const std::string& functionName) {
  if (m_globals.find(functionName) != m_globals.end()) {
    call(m_globals[functionName]);
  } else {
    throw createError("Unknown variable");
  }
}

ff::Ref<ff::Object> ff::VM::returnCall() {
  Ref<Object> result = getStack().canPop() ? pop() : Ref<Object>();
  m_callStack.pop();
  if (m_callStack.size() > 1) {
    m_running = true;
  }
  return result;
}

bool ff::VM::executeInstruction(Opcode op) {
#ifdef _FF_DEBUG_TRACE
  if (config::get("debug") != "0") {
    printf("%04zx | ", getCode()->getReadIndex());
    switch (op) {
      case OP_POP:
        printf("OP_POP\n");
        break;
      case OP_PULL_UP:
        printf("OP_PULL_UP\n");
        break;
      case OP_ROL:
        printf("OP_ROL\n");
        break;
      case OP_DUP:
        printf("OP_DUP\n");
        break;
      case OP_NULL:
        printf("OP_NULL\n");
        break;
      case OP_TRUE:
        printf("OP_TRUE\n");
        break;
      case OP_FALSE:
        printf("OP_FALSE\n");
        break;
      case OP_NEW:
        printf("OP_NEW\n");
        break;
      case OP_COPY:
        printf("OP_COPY\n");
        break;
      case OP_LOAD_CONSTANT:
        printf("OP_LOAD_CONSTANT\n");
        break;
      case OP_NEW_GLOBAL:
        printf("OP_NEW_GLOBAL\n");
        break;
      case OP_GET_GLOBAL:
        printf("OP_GET_GLOBAL\n");
        break;
      case OP_SET_GLOBAL:
        printf("OP_SET_GLOBAL\n");
        break;
      case OP_SET_GLOBAL_REF:
        printf("OP_SET_GLOBAL_REF\n");
        break;
      case OP_GET_LOCAL:
        printf("OP_GET_LOCAL\n");
        break;
      case OP_SET_LOCAL:
        printf("OP_SET_LOCAL\n");
        break;
      case OP_SET_LOCAL_REF:
        printf("OP_SET_LOCAL_REF\n");
        break;
      case OP_GET_FIELD:
        printf("OP_GET_FIELD\n");
        break;
      case OP_SET_FIELD:
        printf("OP_SET_FIELD\n");
        break;
      case OP_SET_FIELD_REF:
        printf("OP_SET_FIELD_REF\n");
        break;
      case OP_GET_STATIC:
        printf("OP_GET_STATIC\n");
        break;
      case OP_JUMP:
        printf("OP_JUMP\n");
        break;
      case OP_JUMP_TRUE:
        printf("OP_JUMP_TRUE\n");
        break;
      case OP_JUMP_FALSE:
        printf("OP_JUMP_FALSE\n");
        break;
      case OP_LOOP:
        printf("OP_LOOP\n");
        break;
      case OP_CALL:
        printf("OP_CALL\n");
        break;
      case OP_CALL_MEMBER:
        printf("OP_CALL_MEMBER\n");
        break;
      case OP_RETURN:
        printf("OP_RETURN\n");
        break;
      case OP_CAST:
        printf("OP_CAST\n");
        break;
      case OP_PRINT:
        printf("OP_PRINT\n");
        break;
      case OP_ADD:
        printf("OP_ADD\n");
        break;
      case OP_SUB:
        printf("OP_SUB\n");
        break;
      case OP_MUL:
        printf("OP_MUL\n");
        break;
      case OP_DIV:
        printf("OP_DIV\n");
        break;
      case OP_MOD:
        printf("OP_MOD\n");
        break;
      case OP_EQ:
        printf("OP_EQ\n");
        break;
      case OP_NEQ:
        printf("OP_NEQ\n");
        break;
      case OP_LT:
        printf("OP_LT\n");
        break;
      case OP_GT:
        printf("OP_GT\n");
        break;
      case OP_LE:
        printf("OP_LE\n");
        break;
      case OP_GE:
        printf("OP_GE\n");
        break;
      case OP_AND:
        printf("OP_AND\n");
        break;
      case OP_OR:
        printf("OP_OR\n");
        break;
      case OP_NOT:
        printf("OP_NOT\n");
        break;
      case OP_NEG:
        printf("OP_NEG\n");
        break;
      case OP_INC:
        printf("OP_INC\n");
        break;
      case OP_DEC:
        printf("OP_DEC\n");
        break;
      case OP_BREAKPOINT:
        printf("OP_BREAKPOINT\n");
        break;
    }
  }
#endif

  switch (op) {
    case OP_POP: {
      pop();
      break;
    }
    case OP_PULL_UP: {
      uint16_t index = getCode()->template read<uint16_t>();
      Ref<Object> value = getStack()[getStack().size() - index];
      getStack().getBuffer().erase(getStack().getBuffer().end() - index);
      push(value);
      break;
    }
    case OP_ROL: {
      Ref<Object> a = pop();
      Ref<Object> b = pop();
      push(a);
      push(b);
      break;
    }
    case OP_DUP: {
      push(getStack().peek());
      break;
    }
    case OP_NULL: {
      push(Ref<Object>());
      break;
    }
    case OP_TRUE: {
      push(Bool::createInstance(true).asRefTo<Object>());
      break;
    }
    case OP_FALSE: {
      push(Bool::createInstance(false).asRefTo<Object>());
      break;
    }
    case OP_NEW: { // ast::NewNode
      throw createError("OP_NEW: Unimplemented");
    }
    case OP_COPY: {
      Ref<Object> object = pop();
      callMember(object, "__copy__", 0);
      break;
    }
    case OP_LOAD_CONSTANT: {
      push(getCode()->getConstant(getCode()->read<uint32_t>()));
      break;
    }
    case OP_NEW_GLOBAL: {
      Ref<String> varName = popCheckType(StringType::getInstance()).asRefTo<String>();
      m_globals[varName->value] = {};
      break;
    }
    case OP_GET_GLOBAL: {
      Ref<String> varName = popCheckType(StringType::getInstance()).asRefTo<String>();
      if (m_globals.find(varName->value) == m_globals.end()) {
        throw createError("Undefined variable '%s'", varName->value.c_str());
      }
      push(m_globals[varName->value]);
      break;
    }
    case OP_SET_GLOBAL: {
      Ref<String> varName = popCheckType(StringType::getInstance()).asRefTo<String>();
      if (m_globals.find(varName->value) == m_globals.end()) {
        throw createError("Undefined variable '%s'", varName->value.c_str());
      }
      m_globals[varName->value] = pop(); // TODO: check: popCheckType(m_globals[varName->value].type); ???
      break;
    }
    case OP_SET_GLOBAL_REF: {
      Ref<String> varName = popCheckType(StringType::getInstance()).asRefTo<String>();
      if (m_globals.find(varName->value) == m_globals.end()) {
        throw createError("Undefined variable '%s'", varName->value.c_str());
      }
      Ref<Object> self = m_globals[varName->value];
      callMember(self, "__assign__", {self, pop()});
      pop();
      break;
    }
    case OP_GET_LOCAL: {
      uint32_t local = getCode()->template read<uint32_t>();
      push(getStack()[local]); // frame?
      break;
    }
    case OP_SET_LOCAL: {
      uint32_t local = getCode()->template read<uint32_t>();
      getStack()[local] = pop();
      break;
    }
    case OP_SET_LOCAL_REF: {
      uint32_t local = getCode()->template read<uint32_t>();
      Ref<Object> self = getStack()[local];
      callMember(self, "__assign__", {self, pop()});
      pop();
      break;
    }
    case OP_GET_FIELD: {
      Ref<String> fieldName = popCheckType(StringType::getInstance()).asRefTo<String>();
      Ref<Object> object = pop();
      push(object->getField(fieldName->value));
      break;
    }
    case OP_SET_FIELD: { // [ name, obj, value ]
      Ref<String> fieldName = popCheckType(StringType::getInstance()).asRefTo<String>();
      Ref<Object> object = pop();
      Ref<Object> value = pop();
      object->setField(fieldName->value, value);
      break;
    }
    case OP_SET_FIELD_REF: { // [ name, obj, value ]
      Ref<String> fieldName = popCheckType(StringType::getInstance()).asRefTo<String>();
      Ref<Object> object = pop();
      Ref<Object> value = pop();
      Ref<Object> self = object->getField(fieldName->value);
      callMember(self, "__assign__", {self, value});
      pop();
      break;
    }
    case OP_GET_STATIC: {
      throw createError("OP_GET_STATIC: Unimplemented");
    }
    case OP_JUMP: {
      uint16_t offset = getCode()->template read<uint16_t>();
      getCode()->setReadIndex(getCode()->getReadIndex() + offset);
      break;
    }
    case OP_JUMP_TRUE: {
      uint16_t offset = getCode()->template read<uint16_t>();
      Ref<Object> object = pop();
      bool result = false;
      if (object.get()) {
        if (object->isType() && object.as<Type>()->getTypeName() != "null") {
          result = true;
        } else if (isOfType(object, BoolType::getInstance())) {
          result = object.as<Bool>()->value;
        } else {
          callMember(object, "__bool__", 0);
          result = popCheckType(BoolType::getInstance()).asRefTo<Bool>()->value;
        }
      }
      if (result) {
        getCode()->setReadIndex(getCode()->getReadIndex() + offset);
      }
      break;
    }
    case OP_JUMP_FALSE: {
      uint16_t offset = getCode()->template read<uint16_t>();
      Ref<Object> object = pop();
      bool result = false;
      if (object.get()) {
        if (object->isType() && object.as<Type>()->getTypeName() != "null") {
          result = true;
        } else if (isOfType(object, BoolType::getInstance())) {
          result = object.as<Bool>()->value;
        } else {
          callMember(object, "__bool__", 0);
          result = popCheckType(BoolType::getInstance()).asRefTo<Bool>()->value;
        }
      }
      if (!result) {
        getCode()->setReadIndex(getCode()->getReadIndex() + offset);
      }
      break;
    }
    case OP_LOOP: {
      uint16_t offset = getCode()->template read<uint16_t>();
      getCode()->setReadIndex(getCode()->getReadIndex() - offset);
      break;
    }
    case OP_CALL: {
      Ref<Object> fn = pop();
      int argc = popCheckType(IntType::getInstance()).asRefTo<Int>()->value;
      call(fn, argc);
      break;
    }
    case OP_CALL_MEMBER: {
      Ref<String> memberName = popCheckType(StringType::getInstance()).asRefTo<String>();
      Ref<Object> object = pop();
      callMember(object, memberName->value, popCheckType(IntType::getInstance()).asRefTo<Int>()->value);
      break;
    }
    case OP_RETURN: {
      auto result = returnCall();
      push(result);
      return false;
    }
    case OP_CAST: {
      Ref<String> typeName = popCheckType(StringType::getInstance()).asRefTo<String>();
      Ref<Object> object = pop();
      push(Object::cast(this, object, typeName->value));
      break;
    }
    case OP_PRINT: {
      Ref<Object> value = pop();
      printf("%s\n", value.get() ? value->toString().c_str() : "null");
      break;
    }
    case OP_ADD: {
      Ref<Object> rhs = pop();
      Ref<Object> lhs = pop();
      callMember(lhs, "__add__", {lhs, rhs});
      break;
    }
    case OP_SUB: {
      Ref<Object> rhs = pop();
      Ref<Object> lhs = pop();
      callMember(lhs, "__sub__", {lhs, rhs});
      break;
    }
    case OP_MUL: {
      Ref<Object> rhs = pop();
      Ref<Object> lhs = pop();
      callMember(lhs, "__mul__", {lhs, rhs});
      break;
    }
    case OP_DIV: {
      Ref<Object> rhs = pop();
      Ref<Object> lhs = pop();
      callMember(lhs, "__div__", {lhs, rhs});
      break;
    }
    case OP_MOD: {
      Ref<Object> rhs = pop();
      Ref<Object> lhs = pop();
      callMember(lhs, "__mod__", {lhs, rhs});
      break;
    }
    case OP_EQ: {
      Ref<Object> rhs = pop();
      Ref<Object> lhs = pop();
      callMember(lhs, "__eq__", {lhs, rhs});
      break;
    }
    case OP_NEQ: {
      Ref<Object> rhs = pop();
      Ref<Object> lhs = pop();
      callMember(lhs, "__neq__", {lhs, rhs});
      break;
    }
    case OP_LT: {
      Ref<Object> rhs = pop();
      Ref<Object> lhs = pop();
      callMember(lhs, "__lt__", {lhs, rhs});
      break;
    }
    case OP_GT: {
      Ref<Object> rhs = pop();
      Ref<Object> lhs = pop();
      callMember(lhs, "__gt__", {lhs, rhs});
      break;
    }
    case OP_LE: {
      Ref<Object> rhs = pop();
      Ref<Object> lhs = pop();
      callMember(lhs, "__le__", {lhs, rhs});
      break;
    }
    case OP_GE: {
      Ref<Object> rhs = pop();
      Ref<Object> lhs = pop();
      callMember(lhs, "__ge__", {lhs, rhs});
      break;
    }
    case OP_AND: {
      Ref<Object> rhs = pop();
      Ref<Object> lhs = pop();

      bool result;
      if (isOfType(lhs, BoolType::getInstance())) {
        result = lhs.as<Bool>()->value;
      } else {
        callMember(lhs, "__bool__", 0);
        result = popCheckType(BoolType::getInstance()).asRefTo<Bool>()->value;
      }

      if (isOfType(rhs, BoolType::getInstance())) {
        result = rhs.as<Bool>()->value;
      } else {
        callMember(rhs, "__bool__", 0);
        result = result && popCheckType(BoolType::getInstance()).asRefTo<Bool>()->value;
      }

      push(Bool::createInstance(result).asRefTo<Object>());
      break;
    }
    case OP_OR: {
      Ref<Object> rhs = pop();
      Ref<Object> lhs = pop();

      bool result;
      if (isOfType(lhs, BoolType::getInstance())) {
        result = lhs.as<Bool>()->value;
      } else {
        callMember(lhs, "__bool__", 0);
        result = popCheckType(BoolType::getInstance()).asRefTo<Bool>()->value;
      }

      if (isOfType(rhs, BoolType::getInstance())) {
        result = result || rhs.as<Bool>()->value;
      } else {
        callMember(rhs, "__bool__", 0);
        result = result || popCheckType(BoolType::getInstance()).asRefTo<Bool>()->value;
      }

      push(Bool::createInstance(result).asRefTo<Object>());
      break;
    }
    case OP_NOT: {
      Ref<Object> operand = pop();
      if (!isOfType(operand, BoolType::getInstance())) {
        callMember(operand, "__bool__", 0);
        operand = popCheckType(BoolType::getInstance());
      }
      callMember(operand, "__not__", {operand});
      break;
    }
    case OP_NEG: {
      Ref<Object> operand = pop();
      callMember(operand, "__neg__", {operand});
      break;
    }
    case OP_INC: {
      Ref<Object> operand = pop();
      callMember(operand, "__inc__", {operand});
      break;
    }
    case OP_DEC: {
      Ref<Object> operand = pop();
      callMember(operand, "__dec__", {operand});
      break;
    }
    case OP_BREAKPOINT: {
#ifdef _DEBUG
      runtimeBreakpoint();
#else
      throw createError("Breakpoints are not supported in release builds (compile with -p debug)");
#endif
      break;
    }
    default:
      throw createError("Unknown Instruction 0x%x", op);
  }

#ifdef _FF_DEBUG_TRACE
  if (config::get("debug") != "0") {
    printf("     ");
    printStack();
  }
#endif

  return true;
}

void ff::VM::printStack() {
  printf("[");
  for (int i = 0; i < getStack().size(); i++) {
    printf("%s", getStack()[i].get() ? getStack()[i]->toString().c_str() : "<null>");
    if (i + 1 < getStack().size()) printf(", ");
  }
  printf("]\n");
}
