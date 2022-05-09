#ifndef _FF_TYPES_NATIVE_FUNCTION_H_
#define _FF_TYPES_NATIVE_FUNCTION_H_ 1

#include <ff/object.h>
#include <ff/types/function.h>
#include <ff/compiler/type_annotation.h>
#include <ff/ref.h>
#include <functional>
#include <vector>
#include <string>

namespace ff {

class VM;

class NativeFunctionType : public Type {
 private:
  static Ref<NativeFunctionType> m_instance;

  NativeFunctionType();

 public:
  ~NativeFunctionType();

  std::string toString() const override;

  static Ref<NativeFunctionType> getInstance();
};

class NativeFunction : public Instance {
 public:
  using ValueType = std::function<Ref<Object>(VM*, std::vector<Ref<Object>>)>; // result (context, args)
  // using ValueType = Ref<Object>(*)(VM*, std::vector<Ref<Object>>); // result (context, args)

  ValueType func;
  std::vector<Function::Argument> args;
  Ref<TypeAnnotation> returnType;

 public:
  NativeFunction(ValueType func, const std::vector<Function::Argument>& args, Ref<TypeAnnotation> returnType);
  ~NativeFunction();

  std::vector<Function::Argument>& getArgs();
  Ref<TypeAnnotation> getReturnType();

  std::string toString() const override;
  bool equals(Ref<Object> other) const override;

  static Ref<NativeFunction> createInstance(ValueType func, const std::vector<Function::Argument>& args, Ref<TypeAnnotation> returnType);
};

} /* namespace ff */

#endif /* _FF_TYPES_NATIVE_FUNCTION_H_ */