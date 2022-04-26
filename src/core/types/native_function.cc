#include <ff/types/native_function.h>
#include <ff/memory.h>
#include <sstream>
// #include <cstdlib>

ff::Ref<ff::NativeFunctionType> ff::NativeFunctionType::m_instance = nullptr;

ff::NativeFunctionType::NativeFunctionType() : Type("native_function") {
  //
}

ff::NativeFunctionType::~NativeFunctionType() {}

std::string ff::NativeFunctionType::toString() const {
  return "native_function";
}

ff::Ref<ff::NativeFunctionType> ff::NativeFunctionType::getInstance() {
  if (!m_instance.get()) {
    m_instance = memory::allocate<NativeFunctionType>();
    new (m_instance.get()) NativeFunctionType();
  }
  return m_instance;
}

ff::NativeFunction::NativeFunction(ValueType func, std::vector<Function::Argument> args)
  : Instance(NativeFunctionType::getInstance().asRefTo<Type>()), func(func), args(args) {}

ff::NativeFunction::~NativeFunction() {}

std::string ff::NativeFunction::toString() const {
  std::stringstream ss;
  ss << (void*)this;
  return "<native function at " + ss.str() + ">";
}

bool ff::NativeFunction::equals(Ref<Object> other) const {
  return other->getObjectType() == OTYPE_INSTANCE
      && other.as<Instance>()->getType() == getType()
      && &other.as<NativeFunction>()->func == &func;
}

ff::Ref<ff::NativeFunction> ff::NativeFunction::createInstance(ValueType func, std::vector<Function::Argument> args) {
  return memory::construct<NativeFunction>(func, args);
}
