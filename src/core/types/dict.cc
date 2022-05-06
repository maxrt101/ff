#include <ff/types/dict.h>
#include <ff/types/string.h>
#include <ff/memory.h>
#include <ff/runtime.h>
#include <ff/types.h>
#include <mrt/strutils.h>
#include <vector>

ff::Ref<ff::DictType> ff::DictType::m_instance;

ff::DictType::DictType() : Type("dict") {
  setField("__as_string__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return String::createInstance(args[0].as<Dict>()->toString()).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("dict")}
  }, TypeAnnotation::create("string")).asRefTo<Object>());

  setField("get", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return args[0].as<Dict>()->getField(args[1].as<String>()->value);
  }, {
    {"self", TypeAnnotation::create("dict")},
    {"key", TypeAnnotation::create("string")}
  }, TypeAnnotation::any()).asRefTo<Object>());

  setField("set", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    args[0].as<Dict>()->setField(args[1].as<String>()->value, args[2]);
    return Ref<Object>();
  }, {
    {"self", TypeAnnotation::create("dict")},
    {"key", TypeAnnotation::create("string")},
    {"value", TypeAnnotation::create("any")}
  }, TypeAnnotation::any()).asRefTo<Object>());

  setField("has", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    auto fields = args[0].as<Dict>()->getFields();
    return Bool::createInstance(fields.find(args[1].as<String>()->value) != fields.end()).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("dict")},
    {"key", TypeAnnotation::create("string")}
  }, TypeAnnotation::create("bool")).asRefTo<Object>());

  setField("size", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Int::createInstance(args[0].as<Dict>()->getFields().size()).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("dict")}
  }, TypeAnnotation::create("int")).asRefTo<Object>());

  setField("__bool__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Bool::createInstance(!args[0].as<Dict>()->getFields().empty()).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("dict")}
  }, TypeAnnotation::create("bool")).asRefTo<Object>());

  setField("__copy__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Dict::createInstance(args[0].as<Dict>()->getFields()).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("dict")}
  }, TypeAnnotation::create("dict")).asRefTo<Object>());

  setField("__assign__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    args[0].as<Dict>()->getFields() = args[1].as<Dict>()->getFields();
    return Ref<Object>();
  }, {
    {"self", TypeAnnotation::create("dict")},
    {"other", TypeAnnotation::create("dict")}
  }, TypeAnnotation::create("dict")).asRefTo<Object>());
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
