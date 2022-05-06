#include <ff/types/int.h>
#include <ff/memory.h>
#include <ff/runtime.h>
#include <ff/types.h>
#include <vector>

#define _DEFINE_BINARY_OP(name, T, op, ret) \
  do { \
    setField(name, NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) { \
      int lhs = args[0].as<Int>()->value; \
      int rhs = 0; \
      if (isOfType(args[1], IntType::getInstance())) { \
        rhs = args[1].as<Int>()->value; \
      } else if (isOfType(args[1], FloatType::getInstance())) { \
        rhs = args[1].as<Float>()->value; \
      } else { \
        rhs = Object::cast(context, args[1], "int").as<Int>()->value; \
      } \
      return T::createInstance(lhs op rhs).asRefTo<Object>(); \
    }, { \
      {"self", TypeAnnotation::create("int")}, \
      {"other", TypeAnnotation::create("any")} \
    }, TypeAnnotation::create(ret)).asRefTo<Object>()); \
  } while (0)

ff::Ref<ff::IntType> ff::IntType::m_instance;

ff::IntType::IntType() : Type("int") {
  _DEFINE_BINARY_OP("__add__", Int, +, "int");
  _DEFINE_BINARY_OP("__sub__", Int, -, "int");
  _DEFINE_BINARY_OP("__mul__", Int, *, "int");
  _DEFINE_BINARY_OP("__div__", Int, /, "int");
  _DEFINE_BINARY_OP("__mod__", Int, %, "int");
  _DEFINE_BINARY_OP("__eq__",  Bool, ==, "bool");
  _DEFINE_BINARY_OP("__neq__", Bool, !=, "bool");
  _DEFINE_BINARY_OP("__lt__",  Bool, <,  "bool");
  _DEFINE_BINARY_OP("__gt__",  Bool, >,  "bool");
  _DEFINE_BINARY_OP("__le__",  Bool, <=, "bool");
  _DEFINE_BINARY_OP("__ge__",  Bool, >=, "bool");

  setField("__neg__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Int::createInstance(-args[0].as<Int>()->value).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("int")}
  }, TypeAnnotation::create("bool")).asRefTo<Object>());

  setField("__inc__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    auto res = Int::createInstance(args[0].as<Int>()->value).asRefTo<Object>();
    args[0].as<Int>()->value++;
    return res;
  }, {
    {"self", TypeAnnotation::create("int")}
  }, TypeAnnotation::create("int")).asRefTo<Object>());

  setField("__dec__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    auto res = Int::createInstance(args[0].as<Int>()->value).asRefTo<Object>();
    args[0].as<Int>()->value--;
    return res;
  }, {
    {"self", TypeAnnotation::create("int")}
  }, TypeAnnotation::create("int")).asRefTo<Object>());

  setField("__as_int__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Int::createInstance(args[0].as<Int>()->value).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("int")}
  }, TypeAnnotation::create("int")).asRefTo<Object>());

  setField("__as_float__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Float::createInstance(args[0].as<Int>()->value).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("int")}
  }, TypeAnnotation::create("float")).asRefTo<Object>());

  setField("__as_string__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return String::createInstance(std::to_string(args[0].as<Int>()->value)).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("int")}
  }, TypeAnnotation::create("string")).asRefTo<Object>());

  setField("__bool__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Bool::createInstance(args[0].as<Int>()->value != 0).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("int")}
  }, TypeAnnotation::create("bool")).asRefTo<Object>());

  setField("__copy__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Int::createInstance(args[0].as<Int>()->value).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("int")}
  }, TypeAnnotation::create("int")).asRefTo<Object>());

  setField("__assign__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    args[0].as<Int>()->value = args[1].as<Int>()->value;
    return Ref<Object>();
  }, {
    {"self", TypeAnnotation::create("int")},
    {"other", TypeAnnotation::create("int")}
  }, TypeAnnotation::create("int")).asRefTo<Object>());
}

ff::IntType::~IntType() {}

std::string ff::IntType::toString() const {
  return "int";
}

ff::Ref<ff::IntType> ff::IntType::getInstance() {
  if (!m_instance.get()) {
    m_instance = memory::allocate<IntType>();
    new (m_instance.get()) IntType();
  }
  return m_instance;
}

ff::Int::Int(ValueType value) : Instance(IntType::getInstance().asRefTo<Type>()), value(value) {
  // setField("testValue", Float::createInstance(10.0).asRefTo<Object>());
}

ff::Int::~Int() {}

std::string ff::Int::toString() const {
  return std::to_string(value);
}

bool ff::Int::equals(Ref<Object> other) const {
  return other->getObjectType() == OTYPE_INSTANCE
      && other.as<Instance>()->getType() == getType()
      && other.as<Int>()->value == value;
}

ff::Ref<ff::Int> ff::Int::createInstance(ValueType value) {
  return memory::construct<Int>(value);
}
