#include <ff/types/function.h>
#include <ff/memory.h>
#include <sstream>

ff::Ref<ff::FunctionType> ff::FunctionType::m_instance;

ff::FunctionType::FunctionType() : Type("function") {
  //
}

ff::FunctionType::~FunctionType() {}

std::string ff::FunctionType::toString() const {
  return "function";
}

ff::Ref<ff::FunctionType> ff::FunctionType::getInstance() {
  if (!m_instance.get()) {
    m_instance = memory::allocate<FunctionType>();
    new (m_instance.get()) FunctionType();
  }
  return m_instance;
}

ff::Function::Function(ValueType code, const std::vector<Argument>& args, Ref<TypeAnnotation> returnType)
  : Instance(FunctionType::getInstance().asRefTo<Type>()), code(code), args(args), returnType(returnType) {}

ff::Function::~Function() {}

std::vector<ff::Function::Argument> ff::Function::getArgs() {
  return args;
}

ff::Ref<ff::TypeAnnotation> ff::Function::getReturnType() {
  return returnType;
}

std::string ff::Function::toString() const {
  std::stringstream ss;
  ss << "function@" << (void*)this << "(";
  for (int i = 0; i < args.size(); i++) {
    ss << args[i].name << ": " << args[i].type->toString();
    if (i + 1 < args.size()) {
      ss << ", ";
    }
  }
  ss << ") -> " << returnType->toString();
  return ss.str();
}

bool ff::Function::equals(Ref<Object> other) const {
  return other->getObjectType() == OTYPE_INSTANCE
      && other.as<Instance>()->getType() == getType()
      && other.as<Function>()->code == code;
}

ff::Ref<ff::Function> ff::Function::createInstance(ValueType code, const std::vector<Argument>& args, Ref<TypeAnnotation> returnType) {
  return memory::construct<Function>(code, args, returnType);
}
