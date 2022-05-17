#include <ff/types/string.h>
#include <ff/memory.h>
#include <ff/runtime.h>
#include <ff/types.h>

#include <mrt/strutils.h>

#include <unordered_map>
#include <vector>
#include <cstdio>

using namespace ff::types;

static std::unordered_map<std::string, ff::Ref<ff::String>> g_strings;

ff::Ref<ff::StringType> ff::StringType::m_instance;

ff::StringType::StringType() : Type("string") {
  setField("__add__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      std::string rhs;
      if (isOfType(args[1], StringType::getInstance())) {
        rhs = strval(args[1]);
      } else if (isOfType(args[1], IntType::getInstance())) {
        rhs = std::to_string(intval(args[1]));
      } else if (isOfType(args[1], FloatType::getInstance())) {
        rhs = std::to_string(floatval(args[1]));
      } else if (isOfType(args[1], BoolType::getInstance())) {
        rhs = boolval(args[1]) ? "true" : "false";
      } else if (args[1].get() == nullptr) {
        rhs = "null";
      } else {
        rhs = strval(Object::cast(context, args[1], "string"));
      }
      return obj(String::createInstance(strval(args[0]) + rhs));
    }, {
      {"self", type("string")},
      {"rhs", any()}
    }, type("string")))
  );

  setField("__eq__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(boolean( strval(args[0]) == strval(args[1]) ));
    }, {
      {"self", type("string")},
      {"other", type("string")}
    }, type("bool")))
  );

  setField("__neq__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(boolean( strval(args[0]) != strval(args[1]) ));
    }, {
      {"self", type("string")},
      {"other", type("string")}
    }, type("bool")))
  );

  setField("slice",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      auto start = intval(args[1]);
      auto end = intval(args[2]);
      return obj(string( strval(args[0]).substr(start, end - start) ));
    }, {
      {"self", type("string")},
      {"start", type("int")},
      {"end", type("int")}
    }, type("bool")))
  );

  setField("starts",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(boolean( mrt::str::startsWith(strval(args[0]), strval(args[1])) ));
    }, {
      {"self", type("string")},
      {"str", type("string")}
    }, type("bool")))
  );

  setField("ends",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(boolean( mrt::str::endsWith(strval(args[0]), strval(args[1])) ));
    }, {
      {"self", type("string")},
      {"str", type("string")}
    }, type("bool")))
  );

  setField("has",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(boolean( strval(args[0]).find(strval(args[1])) != std::string::npos ));
    }, {
      {"self", type("string")},
      {"sub", type("string")}
    }, type("bool")))
  );

  setField("find",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      auto index = strval(args[0]).find(strval(args[1]));
      if (index == std::string::npos) {
        return Ref<Object>();
      }
      return obj(integer(index));
    }, {
      {"self", type("string")},
      {"sub", type("string")}
    }, type("int")))
  );

  setField("rfind",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      auto& sub = strval(args[1]);
      auto index = strval(args[0]).rfind(sub);
      if (index == std::string::npos) {
        return Ref<Object>();
      }
      return obj(integer(index));
    }, {
      {"self", type("string")},
      {"sub", type("string")}
    }, type("int")))
  );

  setField("size",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(integer(strval(args[0]).size()));
    }, {
      {"self", type("string")}
    }, type("int")))
  );

  setField("__bool__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(boolean(strval(args[0]).size() != 0));
    }, {
      {"self", type("string")}
    }, type("bool")))
  );

  setField("__copy__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(string(strval(args[0])));
    }, {
      {"self", type("string")}
    }, type("string")))
  );

  setField("__assign__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      strval(args[0]) = strval(args[1]);
      return Ref<Object>();
    }, {
      {"self", type("string")},
      {"other", type("string")}
    }, type("string")))
  );
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

ff::String::String(const ValueType& value)
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

ff::Ref<ff::String> ff::String::createInstance(const ValueType& value) {
  return memory::construct<String>(value);
}

ff::Ref<ff::String> ff::String::createInstancePool(const ValueType& value) {
  auto itr = g_strings.find(value);
  if (itr == g_strings.end()) {
    g_strings[value] = memory::construct<String>(value);
  }
  return g_strings[value];
}
