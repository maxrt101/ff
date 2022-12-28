#ifndef _FF_TYPES_H_
#define _FF_TYPES_H_ 1

#include <ff/ref.h>
#include <ff/errors.h>
#include <ff/compiler/type_annotation.h>
#include <ff/types/int.h>
#include <ff/types/bool.h>
#include <ff/types/float.h>
#include <ff/types/string.h>
#include <ff/types/function.h>
#include <ff/types/native_function.h>
#include <ff/types/module.h>
#include <ff/types/dict.h>
#include <ff/types/vector.h>
#include <ff/types/class.h>
#include <ff/types/cptr.h>

namespace ff {

class VM;

template <typename O, typename T>
inline bool isOfType(Ref<O> object, Ref<T> type) {
  if (!object.get()) {
    return false;
  }
  if (!object->isInstance()) {
    throw RuntimeError::create("Expected an object");
  }
  return object.template as<Instance>()->getType()->getTypeName() == type.template asRefTo<Type>()->getTypeName();
}

template <typename O>
inline bool isOfType(Ref<O> object, const std::string& typeName) {
  if (!object.get()) {
    return false;
  }
  if (!object->isInstance()) {
    throw RuntimeError::create("Expected an object");
  }
  return object.template as<Instance>()->getType()->getTypeName() == typeName;
}

namespace types {

template <typename T>
inline Ref<Object> obj(Ref<T> o) {
  return o.template asRefTo<Object>();
}

Ref<String> string(const std::string& value);
Ref<String> string(Ref<Object> object);
Ref<Float> floating(Float::ValueType value);
Ref<Float> floating(Ref<Object> object);
Ref<Int> integer(Int::ValueType value);
Ref<Int> integer(Ref<Object> object);
Ref<Bool> boolean(bool value);
Ref<Bool> boolean(Ref<Object> object);
Ref<Function> fn(Function::ValueType code, const std::vector<Function::Argument>& args, Ref<TypeAnnotation> returnType);
Ref<Function> fn(Ref<Object> object);
Ref<NativeFunction> fn(NativeFunction::ValueType func, const std::vector<Function::Argument>& args, Ref<TypeAnnotation> returnType);
Ref<NativeFunction> nativefn(Ref<Object> object);
Ref<Module> module(const std::string& name);
Ref<Module> module(Ref<Object> object);
Ref<Vector> vector(const Vector::ValueType& value);
Ref<Vector> vector(Ref<Object> object);
Ref<Dict> dict(Dict::ValueType value);
Ref<Dict> dict(Ref<Object> object);
Ref<Class> classobj(const std::string& className, std::unordered_map<std::string, Class::Field> fieldInfo, std::unordered_map<std::string, Ref<Object>> methods);
Ref<Class> classobj(Ref<Object> object);
Ref<ClassInstance> instance(Ref<Class> class_);
Ref<ClassInstance> instance(Ref<Object> object);
Ref<CPtr> cptr(CPtr::ValueType value);
Ref<CPtr> cptr(Ref<Object> object);

std::string& strval(Ref<String> value);
std::string& strval(Ref<Object> object);
Float::ValueType& floatval(Ref<Float> value);
Float::ValueType& floatval(Ref<Object> object);
Int::ValueType& intval(Ref<Int> value);
Int::ValueType& intval(Ref<Object> object);
bool& boolval(Ref<Bool> value);
bool& boolval(Ref<Object> object);
Vector::ValueType& vectorval(Ref<Vector> value);
Vector::ValueType& vectorval(Ref<Object> object);
Dict::ValueType& dictval(Ref<Dict> value);
Dict::ValueType& dictval(Ref<Object> object);
CPtr::ValueType& cptrval(Ref<CPtr> value);
CPtr::ValueType& cptrval(Ref<Object> object);

template <typename T>
inline Ref<T> ref(Ref<T>& annotation) {
  annotation->isRef = true;
  return annotation;
}

template <typename T>
inline Ref<TypeAnnotation> type(Ref<T> annotation) {
  return annotation.template asRefTo<TypeAnnotation>();
}

Ref<TypeAnnotation> type(const std::string& typeName, bool isInferred = false);
Ref<FunctionAnnotation> ftype(const std::vector<Ref<TypeAnnotation>>& arguments, Ref<TypeAnnotation> returnType, bool isInferred = false);
Ref<UnionAnnotation> utype(const std::vector<Ref<TypeAnnotation>>& types, bool isInferred = false);

Ref<TypeAnnotation> any();
Ref<TypeAnnotation> nothing();
Ref<TypeAnnotation> type();

namespace literals {

Ref<Int> operator"" _i(unsigned long long value);
Ref<Float> operator"" _f(long double value);
Ref<String> operator"" _s(const char *s, std::size_t len);

Ref<TypeAnnotation> operator"" _ta(const char *s, std::size_t len);

} /* namespace literals */
} /* namespace types */
} /* namespace ff */

#endif /* _FF_TYPES_H_ */