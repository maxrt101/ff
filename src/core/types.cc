#include <ff/runtime.h>

ff::Ref<ff::Object> ff::castToType(VM* context, Ref<Object> object, const std::string& typeName) {
  std::string castFunctionName = "__as_" + typeName + "__";
  // if (object->hasField(castFunctionName)) {
  context->callMember(object, castFunctionName, {object});
  // }
  // throw RuntimeError::createf("No convertion function found for '%s'", typeName.c_str());
  // return object->
  return context->pop();
}