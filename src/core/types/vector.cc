#include <ff/types/vector.h>
#include <ff/types/string.h>
#include <ff/types/int.h>
#include <ff/runtime.h>
#include <ff/memory.h>
#include <ff/types.h>
#include <vector>

ff::Ref<ff::VectorType> ff::VectorType::m_instance = nullptr;

ff::VectorType::VectorType() : Type("vector") {
  setField("__as_string__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return String::createInstance(args[0].as<Vector>()->toString()).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("vector")}
  }, TypeAnnotation::create("string")).asRefTo<Object>());

  setField("get", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    auto self = args[0].as<Vector>();
    int index = args[1].as<Int>()->value;
    if (index > self->value.size()) {
      return Ref<Object>();
    }
    return self->value[index];
  }, {
    {"self", TypeAnnotation::create("vector")},
    {"index", TypeAnnotation::create("int")}
  }, TypeAnnotation::any()).asRefTo<Object>());

  setField("set", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    auto self = args[0].as<Vector>();
    int index = args[1].as<Int>()->value;
    if (index < self->value.size()) {
      self->value[index] = args[2];
    }
    return Ref<Object>();
  }, {
    {"self", TypeAnnotation::create("vector")},
    {"index", TypeAnnotation::create("int")},
    {"value", TypeAnnotation::create("any")}
  }, TypeAnnotation::any()).asRefTo<Object>());

  setField("append", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    args[0].as<Vector>()->value.push_back(args[1]);
    return Ref<Object>();
  }, {
    {"self", TypeAnnotation::create("vector")},
    {"element", TypeAnnotation::create("any")}
  }, TypeAnnotation::any()).asRefTo<Object>());

  setField("remove", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    auto& vec = args[0].as<Vector>()->value;
    vec.erase(vec.begin() + args[1].as<Int>()->value);
    return Ref<Object>();
  }, {
    {"self", TypeAnnotation::create("vector")},
    {"index", TypeAnnotation::create("int")}
  }, TypeAnnotation::any()).asRefTo<Object>());

  setField("index", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    auto& vec = args[0].as<Vector>()->value;
    for (int i = 0; i < vec.size(); i++) {
      if (vec[i]->equals(args[1])) {
        return Int::createInstance(i).asRefTo<Object>();
      }
    }
    return Ref<Object>();
  }, {
    {"self", TypeAnnotation::create("vector")},
    {"value", TypeAnnotation::create("any")}
  }, TypeAnnotation::create("int")).asRefTo<Object>());

  setField("size", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Int::createInstance(args[0].as<Vector>()->value.size()).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("vector")}
  }, TypeAnnotation::create("int")).asRefTo<Object>());

  setField("__bool__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Bool::createInstance(!args[0].as<Vector>()->value.empty()).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("vector")}
  }, TypeAnnotation::create("bool")).asRefTo<Object>());

  setField("__copy__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return Vector::createInstance(args[0].as<Vector>()->value).asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("vector")}
  }, TypeAnnotation::create("vector")).asRefTo<Object>());

  setField("__assign__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    args[0].as<Vector>()->value = args[1].as<Vector>()->value;
    return Ref<Object>();
  }, {
    {"self", TypeAnnotation::create("vector")},
    {"other", TypeAnnotation::create("vector")}
  }, TypeAnnotation::create("vector")).asRefTo<Object>());
}

ff::VectorType::~VectorType() {}

std::string ff::VectorType::toString() const {
  return "vector";
}

ff::Ref<ff::VectorType> ff::VectorType::getInstance() {
  if (!m_instance.get()) {
    m_instance = memory::allocate<VectorType>();
    new (m_instance.get()) VectorType();
  }
  return m_instance;
}

ff::Vector::Vector(ValueType value) : Instance(VectorType::getInstance().asRefTo<Type>()), value(value) {}

ff::Vector::~Vector() {}

std::string ff::Vector::toString() const {
  std::string result = "{";
  for (int i = 0; i < value.size(); i++) {
    result += value[i]->toString();
    if (i + 1 < value.size()) result += ", ";
  }
  return result + "}";
}

bool ff::Vector::equals(Ref<Object> other) const {
  return other->getObjectType() == OTYPE_INSTANCE
      && other.as<Instance>()->getType() == getType()
      && other.as<Vector>()->value == value;
}

ff::Ref<ff::Vector> ff::Vector::createInstance(ValueType value) {
  return memory::construct<Vector>(value);
}

