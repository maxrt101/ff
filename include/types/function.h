#ifndef _FF_TYPES_FUNCTION_H_
#define _FF_TYPES_FUNCTION_H_ 1

#include <ff/compiler/type_annotation.h>
#include <ff/object.h>
#include <ff/stack.h>
#include <ff/code.h>
#include <ff/ref.h>
#include <string>

namespace ff {

class FunctionType : public Type {
 private:
  static Ref<FunctionType> m_instance;

  FunctionType();

 public:
  ~FunctionType();

  std::string toString() const override;

  static Ref<FunctionType> getInstance();
};

class Function : public Instance {
 public:
  using ValueType = Ref<Code>;

  struct Argument {
    std::string name;
    Ref<TypeAnnotation> type;
  };

  struct Context {
    Stack<Ref<Object>> stack;
    size_t codeOffset;
    Ref<Code> code;
  };

  ValueType code;
  std::vector<Argument> args;
  Ref<TypeAnnotation> returnType = TypeAnnotation::create("any");

 public:
  Function(ValueType code, const std::vector<Argument>& args, Ref<TypeAnnotation> returnType);
  ~Function();

  std::vector<Argument> getArgs();
  Ref<TypeAnnotation> getReturnType();

  std::string toString() const override;
  bool equals(Ref<Object> other) const override;

  static Ref<Function> createInstance(ValueType code, const std::vector<Argument>& args, Ref<TypeAnnotation> returnType);
};

} /* namespace ff */

#endif /* _FF_TYPES_STRING_H_ */