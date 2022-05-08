#include <ff/types/dict.h>
#include <ff/types/string.h>
#include <ff/memory.h>
#include <ff/runtime.h>
#include <ff/types.h>
#include <mrt/strutils.h>
#include <vector>

using namespace ff::types;

ff::Ref<ff::DictType> ff::DictType::m_instance;

ff::DictType::DictType() : Type("dict") {
  setField("__as_string__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(string(args[0].as<Dict>()->toString()));
    }, {
      {"self", type("dict")}
    }, type("string")))
  );

  setField("get",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return args[0].as<Dict>()->getField(args[1].as<String>()->value);
    }, {
      {"self", type("dict")},
      {"key", type("string")}
    }, any()))
  );

  setField("set",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      args[0].as<Dict>()->setField(args[1].as<String>()->value, args[2]);
      return Ref<Object>();
    }, {
      {"self", type("dict")},
      {"key", type("string")},
      {"value", any()}
    }, any()))
  );

  setField("has",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      auto fields = args[0].as<Dict>()->getFields();
      return obj(boolean((fields.find(args[1].as<String>()->value) != fields.end())));
    }, {
      {"self", type("dict")},
      {"key", type("string")}
    }, type("bool")))
  );

  setField("size",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(integer(args[0].as<Dict>()->getFields().size()));
    }, {
      {"self", type("dict")}
    }, type("int")))
  );

  setField("__bool__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(boolean(!args[0].as<Dict>()->getFields().empty()));
    }, {
      {"self", type("dict")}
    }, type("bool")))
  );

  setField("__copy__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(dict(args[0].as<Dict>()->getFields()));
    }, {
      {"self", type("dict")}
    }, type("dict")))
  );

  setField("__assign__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      args[0].as<Dict>()->getFields() = args[1].as<Dict>()->getFields();
      return Ref<Object>();
    }, {
      {"self", type("dict")},
      {"other", type("dict")}
    }, type("dict")))
  );
}

ff::DictType::~DictType() {}

std::string ff::DictType::toString() const {
  return "dict";
}

ff::Ref<ff::DictType> ff::DictType::getInstance() {
  if (!m_instance.get()) {
    m_instance = memory::allocate<DictType>();
    new (m_instance.get()) DictType();
  }
  return m_instance;
}

ff::Dict::Dict(ValueType value) : Instance(DictType::getInstance().asRefTo<Type>()) {
  for (auto& p : value) {
    setField(p.first, p.second);
  }
}

ff::Dict::~Dict() {}

std::string ff::Dict::toString() const {
  std::string result = "{";
  int count = 0;
  for (auto& p : getFields()) {
    result += p.first + " -> " + p.second->toString();
    if (count + 1 < getFields().size()) result += ", ";
    count++;
  }
  return result + "}";
}

bool ff::Dict::equals(Ref<Object> other) const {
  return other->getObjectType() == OTYPE_INSTANCE
      && other.as<Instance>()->getType() == getType()
      && other.as<Dict>()->getFields() == getFields();
}

ff::Ref<ff::Dict> ff::Dict::createInstance(ValueType value) {
  return memory::construct<Dict>(value);
}
