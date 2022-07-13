#include <ff/types/class.h>
#include <ff/types/string.h>
#include <ff/utils/macros.h>
#include <ff/runtime.h>
#include <ff/memory.h>
#include <ff/types.h>
#include <mrt/strutils.h>

using namespace ff::types;

ff::Ref<ff::ClassType> ff::ClassType::m_instance;
ff::Ref<ff::ClassInstanceType> ff::ClassInstanceType::m_instance;

ff::ClassType::ClassType() : Type("class") {
  setField("addField", 
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      args[0].as<Class>()->fieldInfo[strval(args[1])] = Class::Field {
        strval(args[1]),
        false,
        args[2]
      };
      return Ref<Object>();
    }, {
      {"self", type("class")},
      {"name", type("string")},
      {"value", any()}
    }, nothing()))
  );

  setField("addMethod", 
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      args[0].as<Class>()->setField(strval(args[1]), args[2]);
      return Ref<Object>();
    }, {
      {"self", type("class")},
      {"name", type("string")},
      {"value", any()}
    }, nothing()))
  );
}

ff::ClassType::~ClassType() {}

std::string ff::ClassType::toString() const {
  return "class";
}

ff::Ref<ff::ClassType> ff::ClassType::getInstance() {
  if (!m_instance.get()) {
    m_instance = memory::allocate<ClassType>();
    new (m_instance.get()) ClassType();
  }
  return m_instance;
}

ff::Class::Class(const std::string& className)
  : Instance(ClassType::getInstance().asRefTo<Type>()), className(className) {}

ff::Class::Class(const std::string& className, std::unordered_map<std::string, Field> fieldInfo, std::unordered_map<std::string, Ref<Object>> methods)
    : Instance(ClassType::getInstance().asRefTo<Type>()), className(className), fieldInfo(fieldInfo) {
  for (auto& [methodName, method] : methods) {
    setField(methodName, method);
  }
}

ff::Class::~Class() {}

std::string ff::Class::toString() const {
  return className;
}

bool ff::Class::equals(Ref<Object> other) const {
  return this == other.get();
}

ff::Ref<ff::Class> ff::Class::createInstance(const std::string& className) {
  return memory::construct<Class>(className);
}

ff::Ref<ff::Class> ff::Class::createInstance(const std::string& className, std::unordered_map<std::string, Field> fieldInfo, std::unordered_map<std::string, Ref<Object>> methods) {
  return memory::construct<Class>(className, fieldInfo, methods);
}

ff::ClassInstanceType::ClassInstanceType() : Type("instance") {
  //
}

ff::ClassInstanceType::~ClassInstanceType() {}

std::string ff::ClassInstanceType::toString() const {
  return "instance";
}

ff::Ref<ff::ClassInstanceType> ff::ClassInstanceType::getInstance() {
  if (!m_instance.get()) {
    m_instance = memory::allocate<ClassInstanceType>();
    new (m_instance.get()) ClassInstanceType();
  }
  return m_instance;
}

ff::ClassInstance::ClassInstance(Ref<Class> class_) : Instance(ClassInstanceType::getInstance().asRefTo<Type>()), m_class(class_) {
  for (auto& [fieldName, fieldInfo] : getClass()->fieldInfo) {
    if (fieldInfo.isStatic) {
      continue;
    }
    if (fieldInfo.initialValue.get()) {
      setField(fieldName, fieldInfo.initialValue);
    } else {
      setField(fieldName, Ref<Object>());
    }
  }
}

ff::ClassInstance::~ClassInstance() {}

ff::Ref<ff::Class> ff::ClassInstance::getClass() const {
  return m_class;
}

std::string ff::ClassInstance::toString() const {
  char buffer[32] = {0};
  snprintf(buffer, 32, "%p", this);
  return getClass()->className + "@0x" + buffer;
}

bool ff::ClassInstance::equals(Ref<Object> other) const {
  return this == other.get();
}

ff::Ref<ff::ClassInstance> ff::ClassInstance::createInstance(Ref<Class> class_) {
  return memory::construct<ClassInstance>(class_);
}
