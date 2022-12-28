#include <ff/types/cptr.h>
#include <ff/memory.h>
#include <ff/runtime.h>
#include <ff/types.h>
#include <vector>

using namespace ff::types;

ff::Ref<ff::CPtrType> ff::CPtrType::m_instance;

ff::CPtrType::CPtrType() : Type("cptr") {
  // _DEFINE_BINARY_OP("__eq__",  Bool, ==, "bool");
  // _DEFINE_BINARY_OP("__neq__", Bool, !=, "bool");

  setField("__as_string__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(string( cptr(args[0])->toString() ));
    }, {
      {"self", type("int")}
    }, type("string")))
  );

  setField("__bool__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(boolean(cptrval(args[0]) != nullptr));
    }, {
      {"self", type("int")}
    }, type("bool")))
  );

  setField("__copy__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      return obj(cptr(cptrval(args[0])));
    }, {
      {"self", type("cptr")}
    }, type("cptr")))
  );

  setField("__assign__",
    obj(fn([](VM* context, std::vector<Ref<Object>> args) {
      cptrval(args[0]) = cptrval(args[1]);
      return Ref<Object>();
    }, {
      {"self", type("cptr")},
      {"other", type("cptr")}
    }, type("cptr")))
  );
}

ff::CPtrType::~CPtrType() {}

std::string ff::CPtrType::toString() const {
  return "cptr";
}

ff::Ref<ff::CPtrType> ff::CPtrType::getInstance() {
  if (!m_instance.get()) {
    m_instance = memory::allocate<CPtrType>();
    new (m_instance.get()) CPtrType();
  }
  return m_instance;
}

ff::CPtr::CPtr(ValueType value) : Instance(CPtrType::getInstance().asRefTo<Type>()), value(value) {}

ff::CPtr::~CPtr() {}

std::string ff::CPtr::toString() const {
  char buffer[64] = {0};
  snprintf(buffer, sizeof buffer, "%p", value);

  return std::string(buffer);
}

bool ff::CPtr::equals(Ref<Object> other) const {
  return other->getObjectType() == OTYPE_INSTANCE
      && other.as<Instance>()->getType() == getType()
      && other.as<CPtr>()->value == value;
}

ff::Ref<ff::CPtr> ff::CPtr::createInstance(ValueType value) {
  return memory::construct<CPtr>(value);
}
