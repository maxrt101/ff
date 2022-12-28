#include <ff/types/int.h>
#include <ff/memory.h>
#include <ff/runtime.h>
#include <ff/types.h>
#include <vector>

using namespace ff::types;

#define _DEFINE_BINARY_OP(name, T, op, ret) \
  do { \
    setField(name, \
      obj(fn([](VM* context, std::vector<Ref<Object>> args) { \
        int lhs = intval(args[0]); \
        int rhs = 0; \
        if (isOfType(args[1], IntType::getInstance())) { \
          rhs = args[1].as<Int>()->value; \
        } else if (isOfType(args[1], FloatType::getInstance())) { \
          rhs = args[1].as<Float>()->value; \
        } else { \
          rhs = intval(Object::cast(context, args[1], "int")); \
        } \
        return obj(T::createInstance(lhs op rhs)); \
      }, { \
        {"self", type("int")}, \
        {"other", any()} \
      }, type(ret))) \
    ); \
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

  setField("__neg__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(integer(-intval(args[0])));
    }, {
      {"self", type("int")}
    }, type("int")))
  );

  setField("__inc__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      auto res = integer(intval(args[0]));
      intval(args[0])++;
      return obj(res);
    }, {
      {"self", type("int")}
    }, type("int")))
  );

  setField("__dec__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      auto res = integer(intval(args[0]));
      intval(args[0])--;
      return obj(res);
    }, {
      {"self", type("int")}
    }, type("int")))
  );

  setField("__as_int__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(integer(intval(args[0])));
    }, {
      {"self", type("int")}
    }, type("int")))
  );

  setField("__as_bool__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(boolean(intval(args[0]) != 0));
    }, {
      {"self", type("int")}
    }, type("bool")))
  );

  setField("__as_float__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(floating(intval(args[0])));
    }, {
      {"self", type("int")}
    }, type("float")))
  );

  setField("__as_string__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(string(std::to_string(intval(args[0]))));
    }, {
      {"self", type("int")}
    }, type("string")))
  );

  setField("__bool__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(boolean(intval(args[0]) != 0));
    }, {
      {"self", type("int")}
    }, type("bool")))
  );

  setField("__copy__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(integer(intval(args[0])));
    }, {
      {"self", type("int")}
    }, type("int")))
  );

  setField("__assign__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      intval(args[0]) = intval(args[1]);
      return Ref<Object>();
    }, {
      {"self", type("int")},
      {"other", type("int")}
    }, type("int")))
  );
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

ff::Int::Int(ValueType value) : Instance(IntType::getInstance().asRefTo<Type>()), value(value) {}

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
