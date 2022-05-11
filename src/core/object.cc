#include <ff/object.h>
#include <ff/types.h>
#include <ff/errors.h>
#include <ff/memory.h>
#include <ff/runtime.h>
#include <cstdio>

ff::Object::Object(ObjectType type) : m_type(type) {}

ff::ObjectType ff::Object::getObjectType() const {
  return m_type;
}

bool ff::Object::isInstance() const {
  return m_type == OTYPE_INSTANCE;
}

bool ff::Object::isType() const {
  return m_type == OTYPE_TYPE;
}

std::string ff::Object::toString() const {
  return (m_type == OTYPE_TYPE ? "Type" : "Instance");
}

bool ff::Object::equals(Ref<Object> other) const {
  return this == other.get();
}

bool ff::Object::hasField(const std::string& key) const {
  return m_fields.find(key) != m_fields.end();
}

ff::Ref<ff::Object> ff::Object::getField(const std::string& key) {
  if (m_fields.find(key) == m_fields.end()) {
    throw RuntimeError::createf("No such field: '%s'", key.c_str());
  }
  return m_fields[key];
}

void ff::Object::setField(const std::string& key, Ref<Object> value) {
  m_fields[key] = value;
}

std::map<std::string, ff::Ref<ff::Object>>& ff::Object::getFields() {
  return m_fields;
}

const std::map<std::string, ff::Ref<ff::Object>>& ff::Object::getFields() const {
  return m_fields;
}

ff::Ref<ff::Object> ff::Object::cast(VM* context, Ref<Object> object, const std::string& typeName) {
  const std::string castFunctionName = "__as_" + typeName + "__";
  context->callMember(object, castFunctionName);
  return context->pop();
}

bool ff::Object::toBool(VM* context, Ref<Object> object) {
  if (object.get()) {
    if (object->isType() && object.as<Type>()->getTypeName() != "null") {
      return true;
    } else if (isOfType(object, BoolType::getInstance())) {
      return types::boolval(object);
    } else {
      context->callMember(object, "__bool__", 0);
      return types::boolval(context->popCheckType(BoolType::getInstance()));
    }
  }
  return false;
}

ff::Type::Type(const std::string& typeName) : Object(OTYPE_TYPE), m_typeName(typeName) {}

std::string ff::Type::getTypeName() const {
  return m_typeName;
}

bool ff::Type::equals(Ref<Object> other) const {
  return other->getObjectType() == OTYPE_TYPE && other.as<Type>()->getTypeName() == getTypeName();
}

ff::Instance::Instance(Ref<Type> type) : Object(OTYPE_INSTANCE), m_type(type) {}

ff::Ref<ff::Type> ff::Instance::getType() const {
  return m_type;
}
