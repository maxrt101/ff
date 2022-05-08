#include <ff/types.h>

ff::Ref<ff::String> ff::types::string(const std::string& value) {
  return String::createInstance(value);
}

ff::Ref<ff::Float> ff::types::floating(Float::ValueType value) {
  return Float::createInstance(value);
}

ff::Ref<ff::Int> ff::types::integer(Int::ValueType value) {
  return Int::createInstance(value);
}

ff::Ref<ff::Bool> ff::types::boolean(bool value) {
  return Bool::createInstance(value);
}

ff::Ref<ff::Function> ff::types::fn(Function::ValueType code, const std::vector<Function::Argument>& args, ff::Ref<TypeAnnotation> returnType) {
  return Function::createInstance(code, args, returnType);
}

ff::Ref<ff::NativeFunction> ff::types::fn(NativeFunction::ValueType func, const std::vector<Function::Argument>& args, ff::Ref<TypeAnnotation> returnType) {
  return NativeFunction::createInstance(func, args, returnType);
}

ff::Ref<ff::Module> ff::types::module(const std::string& name) {
  return Module::createInstance(name);
}

ff::Ref<ff::Vector> ff::types::vector(const Vector::ValueType& value) {
  return Vector::createInstance(value);
}

ff::Ref<ff::Dict> ff::types::dict(Dict::ValueType value) {
  return Dict::createInstance(value);
}

ff::Ref<ff::TypeAnnotation> ff::types::type(const std::string& typeName, bool isInferred) {
  return TypeAnnotation::create(typeName, isInferred, false);
}

ff::Ref<ff::FunctionAnnotation> ff::types::ftype(const std::vector<Ref<TypeAnnotation>>& arguments, Ref<TypeAnnotation> returnType, bool isInferred) {
  return FunctionAnnotation::create(arguments, returnType, isInferred);
}

ff::Ref<ff::UnionAnnotation> ff::types::utype(const std::vector<Ref<TypeAnnotation>>& types, bool isInferred) {
  return UnionAnnotation::create(types, isInferred);
}

ff::Ref<ff::TypeAnnotation> ff::types::any() {
  return TypeAnnotation::any();
}

ff::Ref<ff::TypeAnnotation> ff::types::nothing() {
  return TypeAnnotation::nothing();
}

ff::Ref<ff::TypeAnnotation> ff::types::type() {
  return TypeAnnotation::type();
}

ff::Ref<ff::Int> ff::types::literals::operator"" _i(unsigned long long value) {
  return Int::createInstance(value);
}

ff::Ref<ff::Float> ff::types::literals::operator"" _f(long double value) {
  return Float::createInstance(value);
}

ff::Ref<ff::String> ff::types::literals::operator"" _s(const char *s, std::size_t len) {
  return String::createInstance(std::string(s, len));
}

ff::Ref<ff::TypeAnnotation> ff::types::literals::operator"" _ta(const char *s, std::size_t len) {
  return TypeAnnotation::create(std::string(s, len));
}
