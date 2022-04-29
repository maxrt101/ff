#include <ff/types/string.h>
#include <ff/memory.h>
#include <ff/runtime.h>
#include <ff/types.h>
#include <unordered_map>
#include <vector>
#include <cstdio>

static std::unordered_map<std::string, ff::Ref<ff::String>> g_strings;

ff::Ref<ff::StringType> ff::StringType::m_instance = nullptr;

ff::StringType::StringType() : Type("string") {
  setField("__add__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    std::string rhs;
    if (isOfType(args[1], StringType::getInstance())) {
      rhs = args[1].as<String>()->value;
    } else if (isOfType(args[1], IntType::getInstance())) {
      rhs = std::to_string(args[1].as<Int>()->value);
    } else if (isOfType(args[1], FloatType::getInstance())) {
      rhs = std::to_string(args[1].as<Float>()->value);
    } else {
      rhs = Object::cast(context, args[1], "int").as<Int>()->value;
    }
    return String::createInstance(args[0].as<String>()->value + rhs).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("string")},
    {"rhs", TypeAnnotation::any()}
  }, TypeAnnotation::create("string")).asRefTo<Object>());

  setField("__eq__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Bool::createInstance(false).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("string")},
    {"rhs", TypeAnnotation::any()}
  }, TypeAnnotation::create("bool")).asRefTo<Object>());

  setField("__bool__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Bool::createInstance(args[0].as<String>()->value.size() != 0).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("string")}
  }, TypeAnnotation::create("bool")).asRefTo<Object>());

  setField("size", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Int::createInstance(args[0].as<String>()->value.size()).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("string")}
  }, TypeAnnotation::create("int")).asRefTo<Object>());

  setField("__copy__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return String::createInstance(args[0].as<String>()->value).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("string")}
  }, TypeAnnotation::create("string")).asRefTo<Object>());
}

ff::StringType::~StringType() {}

std::string ff::StringType::toString() const {
  return "string";
}

ff::Ref<ff::StringType> ff::StringType::getInstance() {
  if (!m_instance.get()) {
    m_instance = memory::allocate<StringType>();
    new (m_instance.get()) StringType();
  }
  return m_instance;
}

ff::String::String(ValueType value)
  : Instance(StringType::getInstance().asRefTo<Type>()), value(value) {}

ff::String::~String() {}

std::string ff::String::toString() const {
  return value;
}

bool ff::String::equals(Ref<Object> other) const {
  return other->getObjectType() == OTYPE_INSTANCE
      && other.as<Instance>()->getType() == getType()
      && other.as<String>()->value == value;
}

ff::Ref<ff::String> ff::String::createInstance(ValueType value) {
  return memory::construct<String>(value);
}

ff::Ref<ff::String> ff::String::createInstancePool(ValueType value) {
  auto itr = g_strings.find(value);
  if (itr == g_strings.end()) {
    g_strings[value] = memory::construct<String>(value);
  }
  return g_strings[value];
}
