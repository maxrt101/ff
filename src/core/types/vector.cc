#include <ff/types/vector.h>
#include <ff/types/string.h>
#include <ff/types/int.h>
#include <ff/runtime.h>
#include <ff/memory.h>
#include <ff/types.h>
#include <vector>

using namespace ff::types;

ff::Ref<ff::VectorType> ff::VectorType::m_instance;

ff::VectorType::VectorType() : Type("vector") {
  setField("__as_string__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(string(args[0].as<Vector>()->toString()));
    }, {
      {"self", type("vector")}
    }, type("string")))
  );

  setField("__neq__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      auto& self = args[0].as<Vector>()->value;
      auto& other = args[1].as<Vector>()->value;
      if (self.size() != other.size()) {
        return obj(boolean(true));
      }
      for (int i = 0; i < self.size(); i++) {
        if (self[i]->equals(other[i])) {
          return obj(boolean(false));
        }
      }
      return obj(boolean(true));
    }, {
      {"self", type("vector")},
      {"other", type("vector")}
    }, type("bool")))
  );

  setField("__eq__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      auto& self = args[0].as<Vector>()->value;
      auto& other = args[1].as<Vector>()->value;
      if (self.size() != other.size()) {
        return obj(boolean(false));
      }
      for (int i = 0; i < self.size(); i++) {
        if (!self[i]->equals(other[i])) {
          return obj(boolean(false));
        }
      }
      return obj(boolean(true));
    }, {
      {"self", type("vector")},
      {"other", type("vector")}
    }, type("bool")))
  );

  setField("get",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      auto self = args[0].as<Vector>();
      int index = args[1].as<Int>()->value;
      if ((index >= 0 && index >= self->value.size()) || (index < 0 && -index >= self->value.size())) {
        return Ref<Object>();
      }
      return self->value[index < 0 ? self->value.size() + index : index];
    }, {
      {"self", type("vector")},
      {"index", type("int")}
    }, any()))
  );

  setField("set",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      auto self = args[0].as<Vector>();
      int index = args[1].as<Int>()->value;
      if ((index >= 0 && index < self->value.size()) || (index < 0 && -index < self->value.size())) {
        self->value[index < 0 ? self->value.size() + index : index] = args[2];
      }
      return Ref<Object>();
    }, {
      {"self", type("vector")},
      {"index", type("int")},
      {"value", any()}
    }, any()))
  );

  setField("append",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      args[0].as<Vector>()->value.push_back(args[1]);
      return Ref<Object>();
    }, {
      {"self", type("vector")},
      {"value", any()}
    }, any()))
  );

  setField("pop",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      Ref<Object> result = args[0].as<Vector>()->value.back();
      args[0].as<Vector>()->value.pop_back();
      return result;
    }, {
      {"self", type("vector")}
    }, any()))
  );

  setField("remove",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      auto& vec = args[0].as<Vector>()->value;
      auto index = intval(args[1]);
      if (index < vec.size()) {
        vec.erase(vec.begin() + index);
      }
      return Ref<Object>();
    }, {
      {"self", type("vector")},
      {"index", type("int")}
    }, any()))
  );

  setField("find",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      auto& vec = args[0].as<Vector>()->value;
      for (int i = 0; i < vec.size(); i++) {
        if (vec[i]->equals(args[1])) {
          return obj(integer(i));
        }
      }
      return Ref<Object>();
    }, {
      {"self", type("vector")},
      {"value", any()}
    }, type("int")))
  );

  setField("contains",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      auto& vec = args[0].as<Vector>()->value;
      for (int i = 0; i < vec.size(); i++) {
        if (vec[i]->equals(args[1])) {
          return obj(boolean(true));
        }
      }
      return obj(boolean(false));
    }, {
      {"self", type("vector")},
      {"value", any()}
    }, type("bool")))
  );

  setField("size",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(integer(args[0].as<Vector>()->value.size()));
    }, {
      {"self", type("vector")}
    }, type("int")))
  );

  setField("__add__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      std::vector<Ref<Object>> res;
      res.insert(res.end(), args[0].as<Vector>()->value.begin(), args[0].as<Vector>()->value.end());
      res.insert(res.end(), args[1].as<Vector>()->value.begin(), args[1].as<Vector>()->value.end());
      return obj(vector(res));
    }, {
      {"self", type("vector")},
      {"other", type("vector")}
    }, type("vector")))
  );

  setField("unique",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      std::vector<Ref<Object>> res;
      auto& vec = args[0].as<Vector>()->value;
      for (auto itr = vec.begin(); itr != vec.end(); ++itr) {
        auto ritr = std::find_if(res.begin(), res.end(), [&itr](auto& element) { return element->equals(*itr); });
        if (ritr == res.end()) {
          res.push_back(*itr);
        }
      }
      return obj(vector(res));
    }, {
      {"self", type("vector")}
    }, type("vector")))
  );

  setField("__bool__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(boolean(!args[0].as<Vector>()->value.empty()));
    }, {
      {"self", type("vector")}
    }, type("bool")))
  );

  setField("__copy__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(vector(args[0].as<Vector>()->value));
    }, {
      {"self", type("vector")}
    }, type("vector")))
  );

  setField("__assign__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      args[0].as<Vector>()->value = args[1].as<Vector>()->value;
      return Ref<Object>();
    }, {
      {"self", type("vector")},
      {"other", type("vector")}
    }, type("vector")))
  );
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

ff::Vector::Vector(const ValueType& value) : Instance(VectorType::getInstance().asRefTo<Type>()), value(value) {}

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

ff::Ref<ff::Vector> ff::Vector::createInstance(const ValueType& value) {
  return memory::construct<Vector>(value);
}

