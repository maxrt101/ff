#include <ff/types/float.h>
#include <ff/memory.h>
#include <ff/runtime.h>
#include <ff/types.h>
#include <vector>

using namespace ff::types;

#define _DEFINE_BINARY_OP(name, T, op, ret) \
  do { \
    setField(name, \
      obj(fn([](VM* context, std::vector<Ref<Object>> args) { \
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
        {"self", type("float")}, \
        {"other", any()} \
      }, type(ret))) \
    ); \
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

  setField("__neg__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(floating(-args[0].as<Float>()->value));
    }, {
      {"self", type("float")}
    }, type("float")))
  );

  setField("__inc__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      auto res = floating(args[0].as<Float>()->value);
      args[0].as<Float>()->value++;
      return obj(res);
    }, {
      {"self", type("float")}
    }, type("float")))
  );

  setField("__dec__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      auto res = floating(args[0].as<Float>()->value);
      args[0].as<Float>()->value--;
      return obj(res);
    }, {
      {"self", type("float")}
    }, type("float")))
  );

  setField("__as_int__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(integer(args[0].as<Float>()->value));
    }, {
      {"self", type("float")}
    }, type("float")))
  );

  setField("__as_float__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(floating(args[0].as<Float>()->value));
    }, {
      {"self", type("float")}
    }, type("float")))
  );

  setField("__as_string__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(string(std::to_string(args[0].as<Float>()->value)));
    }, {
      {"self", type("float")}
    }, type("string")))
  );

  setField("__bool__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(boolean(args[0].as<Float>()->value != 0));
    }, {
      {"self", type("float")}
    }, type("string")))
  );

  setField("__copy__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(floating(args[0].as<Float>()->value));
    }, {
      {"self", type("float")}
    }, type("float")))
  );

  setField("__assign__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      args[0].as<Float>()->value = args[1].as<Float>()->value;
      return Ref<Object>();
    }, {
      {"self", type("float")}
    }, type("float")))
  );
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
