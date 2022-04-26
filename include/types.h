#ifndef _FF_TYPES_H_
#define _FF_TYPES_H_ 1

#include <ff/errors.h>
#include <ff/types/int.h>
#include <ff/types/bool.h>
#include <ff/types/float.h>
#include <ff/types/string.h>
#include <ff/types/function.h>
#include <ff/types/native_function.h>

namespace ff {

class VM;

template <typename O, typename T>
inline bool isOfType(Ref<O> object, Ref<T> type) {
  if (!object.get()) {
    throw RuntimeError::create("Null Pointer");
  }
  if (!object->isInstance()) {
    throw RuntimeError::create("Expected an object");
  }
  return object.template as<Instance>()->getType()->getTypeName() == type.template asRefTo<Type>()->getTypeName();
}

template <typename O>
inline bool isOfType(Ref<O> object, const std::string& typeName) {
  if (!object.get()) {
    throw RuntimeError::create("Null Pointer");
  }
  if (!object->isInstance()) {
    throw RuntimeError::create("Expected an object");
  }
  return object.template as<Instance>()->getType()->getTypeName() == typeName;
}

Ref<Object> castToType(VM* context, Ref<Object> object, const std::string& typeName);

} /* namespace ff */

#endif /* _FF_TYPES_H_ */