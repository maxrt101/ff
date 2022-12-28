#include <ff/types.h>

ff::Ref<ff::String> ff::types::string(const std::string& value) {
  return String::createInstance(value);
}

ff::Ref<ff::String> ff::types::string(Ref<Object> object) {
  return object.asRefTo<String>();
}

ff::Ref<ff::Float> ff::types::floating(Float::ValueType value) {
  return Float::createInstance(value);
}

ff::Ref<ff::Float> ff::types::floating(Ref<Object> object) {
  return object.asRefTo<Float>();
}

ff::Ref<ff::Int> ff::types::integer(Int::ValueType value) {
  return Int::createInstance(value);
}

ff::Ref<ff::Int> ff::types::integer(Ref<Object> object) {
  return object.asRefTo<Int>();
}

ff::Ref<ff::Bool> ff::types::boolean(bool value) {
  return Bool::createInstance(value);
}

ff::Ref<ff::Bool> ff::types::boolean(Ref<Object> object) {
  return object.asRefTo<Bool>();
}

ff::Ref<ff::Function> ff::types::fn(Function::ValueType code, const std::vector<Function::Argument>& args, ff::Ref<TypeAnnotation> returnType) {
  return Function::createInstance(code, args, returnType);
}

ff::Ref<ff::Function> ff::types::fn(Ref<Object> object) {
  return object.asRefTo<Function>();
}

ff::Ref<ff::NativeFunction> ff::types::fn(NativeFunction::ValueType func, const std::vector<Function::Argument>& args, ff::Ref<TypeAnnotation> returnType) {
  return NativeFunction::createInstance(func, args, returnType);
}

ff::Ref<ff::NativeFunction> ff::types::nativefn(Ref<Object> object) {
  return object.asRefTo<NativeFunction>();
}

ff::Ref<ff::Module> ff::types::module(const std::string& name) {
  return Module::createInstance(name);
}

ff::Ref<ff::Module> ff::types::module(Ref<Object> object) {
  return object.asRefTo<Module>();
}

ff::Ref<ff::Vector> ff::types::vector(const Vector::ValueType& value) {
  return Vector::createInstance(value);
}

ff::Ref<ff::Vector> ff::types::vector(Ref<Object> object) {
  return object.asRefTo<Vector>();
}

ff::Ref<ff::Dict> ff::types::dict(Dict::ValueType value) {
  return Dict::createInstance(value);
}

ff::Ref<ff::Dict> ff::types::dict(Ref<Object> object) {
  return object.asRefTo<Dict>();
}

ff::Ref<ff::Class> ff::types::classobj(const std::string& className, std::unordered_map<std::string, Class::Field> fieldInfo, std::unordered_map<std::string, Ref<Object>> methods) {
  return Class::createInstance(className, fieldInfo, methods);
}

ff::Ref<ff::Class> ff::types::classobj(Ref<Object> object) {
  return object.asRefTo<Class>();
}

ff::Ref<ff::ClassInstance> ff::types::instance(Ref<Class> class_) {
  return ClassInstance::createInstance(class_);
}

ff::Ref<ff::ClassInstance> ff::types::instance(Ref<Object> object) {
  return object.asRefTo<ClassInstance>();
}

ff::Ref<ff::CPtr> ff::types::cptr(CPtr::ValueType value) {
  return CPtr::createInstance(value);
}

ff::Ref<ff::CPtr> ff::types::cptr(Ref<Object> object) {
  return object.asRefTo<CPtr>();
}

std::string& ff::types::strval(Ref<String> value) {
  return value->value;
}

std::string& ff::types::strval(Ref<Object> object) {
  return string(object)->value;
}

ff::Float::ValueType& ff::types::floatval(Ref<Float> value) {
  return value->value;
}

ff::Float::ValueType& ff::types::floatval(Ref<Object> object) {
  return floating(object)->value;
}

ff::Int::ValueType& ff::types::intval(Ref<Int> value) {
  return value->value;
}

ff::Int::ValueType& ff::types::intval(Ref<Object> object) {
  return integer(object)->value;
}

bool& ff::types::boolval(Ref<Bool> value) {
  return value->value;
}

bool& ff::types::boolval(Ref<Object> object) {
  return boolean(object)->value;
}

ff::Vector::ValueType& ff::types::vectorval(Ref<Vector> value) {
  return value->value;
}

ff::Vector::ValueType& ff::types::vectorval(Ref<Object> object) {
  return vector(object)->value;
}

ff::Dict::ValueType& ff::types::dictval(Ref<Dict> value) {
  return value->getFields();
}

ff::Dict::ValueType& ff::types::dictval(Ref<Object> object) {
  return dict(object)->getFields();
}

ff::CPtr::ValueType& ff::types::cptrval(Ref<CPtr> value) {
  return value->value;
}

ff::CPtr::ValueType& ff::types::cptrval(Ref<Object> object) {
  return cptr(object)->value;
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
