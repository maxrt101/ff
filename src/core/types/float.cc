#include <ff/types/float.h>
#include <ff/memory.h>
#include <ff/runtime.h>
#include <ff/types.h>
#include <vector>

#define _DEFINE_BINARY_OP(name, T, op, ret) \
  do { \
    setField(name, NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) { \
      float lhs = args[0].as<Float>()->value; \
      float rhs = 0; \
      if (isOfType(args[1], FloatType::getInstance())) { \
        rhs = args[1].as<Float>()->value; \
      } else if (isOfType(args[1], IntType::getInstance())) { \
        rhs = args[1].as<Int>()->value; \
      } else { \
        rhs = Object::cast(context, args[1], "float").as<Float>()->value; \
      } \
      return T::createInstance(lhs op rhs).asRefTo<Object>(); \
    }, { \
      {"self", TypeAnnotation::create("float")}, \
      {"other", TypeAnnotation::any()} \
    }, TypeAnnotation::create(ret)).asRefTo<Object>()); \
  } while (0)

ff::Ref<ff::FloatType> ff::FloatType::m_instance;

ff::FloatType::FloatType() : Type("float") {
  _DEFINE_BINARY_OP("__add__", Float, +, "float");
  _DEFINE_BINARY_OP("__sub__", Float, -, "float");
  _DEFINE_BINARY_OP("__mul__", Float, *, "float");
  _DEFINE_BINARY_OP("__div__", Float, /, "float");
  _DEFINE_BINARY_OP("__eq__",  Bool, ==, "bool");
  _DEFINE_BINARY_OP("__neq__", Bool, !=, "bool");
  _DEFINE_BINARY_OP("__lt__",  Bool, <,  "bool");
  _DEFINE_BINARY_OP("__gt__",  Bool, >,  "bool");
  _DEFINE_BINARY_OP("__le__",  Bool, <=, "bool");
  _DEFINE_BINARY_OP("__ge__",  Bool, >=, "bool");

  setField("__neg__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Float::createInstance(-args[0].as<Float>()->value).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("float")}
  }, TypeAnnotation::create("bool")).asRefTo<Object>());

  setField("__as_int__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Int::createInstance(args[0].as<Float>()->value).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("float")}
  }, TypeAnnotation::create("int")).asRefTo<Object>());

  setField("__as_float__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Float::createInstance(args[0].as<Float>()->value).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("float")}
  }, TypeAnnotation::create("float")).asRefTo<Object>());

  setField("__as_string__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return String::createInstance(std::to_string(args[0].as<Float>()->value)).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("float")}
  }, TypeAnnotation::create("string")).asRefTo<Object>());

  setField("__bool__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Bool::createInstance(args[0].as<Float>()->value != 0.0).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("float")}
  }, TypeAnnotation::create("bool")).asRefTo<Object>());

  setField("__copy__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Float::createInstance(args[0].as<Float>()->value).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("float")}
  }, TypeAnnotation::create("float")).asRefTo<Object>());

  setField("__assign__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    args[0].as<Float>()->value = args[1].as<Float>()->value;
    return Ref<Object>();
  }, {
    {"self", TypeAnnotation::create("float")},
    {"other", TypeAnnotation::create("float")}
  }, TypeAnnotation::create("float")).asRefTo<Object>());
}

ff::FloatType::~FloatType() {}

std::string ff::FloatType::toString() const {
  return "float";
}

ff::Ref<ff::FloatType> ff::FloatType::getInstance() {
  if (!m_instance.get()) {
    m_instance = memory::allocate<FloatType>();
    new (m_instance.get()) FloatType();
  }
  return m_instance;
}

ff::Float::Float(ValueType value)
  : Instance(FloatType::getInstance().asRefTo<Type>()), value(value) {}

ff::Float::~Float() {}

std::string ff::Float::toString() const {
  return std::to_string(value);
}

bool ff::Float::equals(Ref<Object> other) const {
  return other->getObjectType() == OTYPE_INSTANCE
      && other.as<Instance>()->getType() == getType()
      && other.as<Float>()->value == value;
}

ff::Ref<ff::Float> ff::Float::createInstance(ValueType value) {
  return memory::construct<Float>(value);
}
