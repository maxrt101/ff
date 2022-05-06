#include <ff/types/bool.h>
#include <ff/memory.h>
#include <ff/runtime.h>
#include <ff/types.h>
#include <vector>

static ff::Ref<ff::Bool> g_true = ff::Bool::createInstance(true);
static ff::Ref<ff::Bool> g_false = ff::Bool::createInstance(false);

ff::Ref<ff::BoolType> ff::BoolType::m_instance;

ff::BoolType::BoolType() : Type("bool") {
  setField("__not__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return (args[0].as<Bool>()->value ? g_false : g_true).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("bool")}
  }, TypeAnnotation::create("bool")).asRefTo<Object>());

  setField("__as_string__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return String::createInstance(args[0].as<Bool>()->value ? "true" : "false").asRefTo<Object>();
  }, {{
    "self", TypeAnnotation::create("bool")}
  }, TypeAnnotation::create("string")).asRefTo<Object>());

  setField("__copy__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Bool::createInstance(args[0].as<Bool>()->value).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("bool")}
  }, TypeAnnotation::create("bool")).asRefTo<Object>());

  setField("__assign__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    args[0].as<Bool>()->value = args[1].as<Bool>()->value;
    return Ref<Object>();
  }, {
    {"self", TypeAnnotation::create("bool")},
    {"other", TypeAnnotation::create("bool")}
  }, TypeAnnotation::nothing()).asRefTo<Object>());
}

ff::BoolType::~BoolType() {}

std::string ff::BoolType::toString() const {
  return "bool";
}

ff::Ref<ff::BoolType> ff::BoolType::getInstance() {
    if (!m_instance.get()) {
    m_instance = memory::allocate<BoolType>();
    new (m_instance.get()) BoolType();
  }
  return m_instance;
}

ff::Bool::Bool(ValueType value) : Instance(BoolType::getInstance().asRefTo<Type>()), value(value) {}

ff::Bool::~Bool() {}

std::string ff::Bool::toString() const {
  return value ? "true" : "false";
}

bool ff::Bool::equals(ff::Ref<ff::Object> other) const {
  return other->getObjectType() == OTYPE_INSTANCE
      && other.as<Instance>()->getType() == getType()
      && other.as<Bool>()->value == value;
}

ff::Ref<ff::Bool> ff::Bool::createInstance(ValueType value) {
  return memory::construct<Bool>(value);
}

ff::Ref<ff::Bool> ff::Bool::getTrue() {
  return g_true;
}

ff::Ref<ff::Bool> ff::Bool::getFalse() {
  return g_false;
}
