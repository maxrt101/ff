#include <ff/compiler/type_annotation.h>
#include <ff/memory.h>

static ff::Ref<ff::TypeAnnotation> g_any = ff::TypeAnnotation::create("any");
static ff::Ref<ff::TypeAnnotation> g_nothing = ff::TypeAnnotation::create("nothing");
static ff::Ref<ff::TypeAnnotation> g_type = ff::TypeAnnotation::create("type");

ff::TypeAnnotation::TypeAnnotation(Type type, bool isInferred, bool isRef) : annotationType(type), isInferred(isInferred), isRef(isRef) {}

ff::TypeAnnotation::TypeAnnotation(const std::string& typeName, bool isInferred, bool isRef) : annotationType(TATYPE_DEFAULT), typeName(typeName), isInferred(isInferred), isRef(isRef) {}

bool ff::TypeAnnotation::operator==(const TypeAnnotation& rhs) const {
  return type == rhs.type && typeName == rhs.typeName;
}

bool ff::TypeAnnotation::operator!=(const TypeAnnotation& rhs) const {
  return !(*this == rhs);
}

std::string ff::TypeAnnotation::toString() const {
  std::string ref = isRef ? "ref " : "";
  return ref + typeName;
}

ff::Ref<ff::TypeAnnotation> ff::TypeAnnotation::copy() {
  return TypeAnnotation::create(typeName, isInferred, isRef);
}

ff::Ref<ff::TypeAnnotation> ff::TypeAnnotation::create(const std::string& typeName, bool isInferred, bool isRef) {
  return memory::construct<TypeAnnotation>(typeName, isInferred, isRef);
}

ff::Ref<ff::TypeAnnotation> ff::TypeAnnotation::any() {
  return g_any;
}

ff::Ref<ff::TypeAnnotation> ff::TypeAnnotation::nothing() {
  return g_nothing;
}

ff::Ref<ff::TypeAnnotation> ff::TypeAnnotation::type() {
  return g_type;
}

ff::FunctionAnnotation::FunctionAnnotation(const std::vector<Ref<TypeAnnotation>>& arguments, Ref<TypeAnnotation> returnType, bool isInferred)
  : TypeAnnotation(TypeAnnotation::TATYPE_FUNCTION, isInferred), arguments(arguments), returnType(returnType) {}

bool ff::FunctionAnnotation::operator==(const TypeAnnotation& rhs) const {
  if (annotationType != rhs.annotationType) return false;

  auto rhsf = (const FunctionAnnotation&) rhs;

  if (arguments.size() != rhsf.arguments.size()) return false;

  for (int i = 0; i < arguments.size(); i++) {
    if (*arguments[i] != *rhsf.arguments[i]) return false;
  }

  return *returnType == *rhsf.returnType;
}

std::string ff::FunctionAnnotation::toString() const {
  std::string result = "(";

  for (size_t i = 0; i < arguments.size(); i++) {
    result += arguments[i]->toString();
    if (i + 1 < arguments.size()) {
      result += ", ";
    }
  }

  result += ") -> " + returnType->toString();

  return result;
}

ff::Ref<ff::TypeAnnotation> ff::FunctionAnnotation::copy() {
  return FunctionAnnotation::create(arguments, returnType, isInferred).asRefTo<TypeAnnotation>();
}

ff::Ref<ff::FunctionAnnotation> ff::FunctionAnnotation::create(const std::vector<Ref<TypeAnnotation>>& arguments, Ref<TypeAnnotation> returnType, bool isInferred) {
  return memory::construct<FunctionAnnotation>(arguments, returnType, isInferred);
}

ff::UnionAnnotation::UnionAnnotation(const std::vector<ff::Ref<ff::TypeAnnotation>>& types, bool isInferred)
  : TypeAnnotation(TypeAnnotation::TATYPE_UNION, isInferred), types(types) {}

bool ff::UnionAnnotation::operator==(const TypeAnnotation& rhs) const {
  if (rhs.annotationType == TATYPE_UNION) {
    auto rhsu = (const UnionAnnotation&) rhs;

    if (types.size() != rhsu.types.size()) return false;

    for (int i = 0; i < types.size(); i++) {
      if (*types[i] != *rhsu.types[i]) return false;
    }

    return true;
  } else {
    for (int i = 0; i < types.size(); i++) {
      if (*types[i] == rhs) return true;
    }

    return false;
  }
}

std::string ff::UnionAnnotation::toString() const {
  std::string result;

  for (size_t i = 0; i < types.size(); i++) {
    result += types[i]->toString();
    if (i + 1 < types.size()) {
      result += " | ";
    }
  }

  return result;
}

ff::Ref<ff::TypeAnnotation> ff::UnionAnnotation::copy() {
  return UnionAnnotation::create(types, isInferred).asRefTo<TypeAnnotation>();
}

ff::Ref<ff::UnionAnnotation> ff::UnionAnnotation::create(const std::vector<Ref<TypeAnnotation>>& types, bool isInferred) {
  return memory::construct<UnionAnnotation>(types, isInferred);
}
