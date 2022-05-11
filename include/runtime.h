#ifndef _FF_RUNTIME_H_
#define _FF_RUNTIME_H_ 1

#include <ff/errors.h>
#include <ff/object.h>
#include <ff/types.h>
#include <ff/stack.h>
#include <ff/code.h>
#include <ff/ref.h>
#include <cstdarg>
#include <string>
#include <map>

namespace ff {

class VM {
 public:
  using StackType = Ref<Object>;

  struct CallFrame {
    Function::Context context;
  };

 private:
  Stack<CallFrame> m_callStack;
  std::map<std::string, Ref<Object>> m_globals;
  bool m_running = false;
  bool m_requestStop = false;
  int m_returnCode = 0;

 public:
  VM();
  ~VM();

  void run(Ref<Code> code);
  void runMain(Ref<Code> code);
  void stop();

  int getReturnCode();
  void setReturnCode(int returnCode);

  Stack<StackType>& getStack();
  std::map<std::string, Ref<Object>>& getGlobals();

  void push(Ref<Object> obj);
  Ref<Object> pop();
  std::vector<Ref<Object>> pop(int count, bool reverse = false);
  std::vector<Ref<Object>> popFrom(int frameOffset, int count);

  void jumpForward(uint32_t offset);
  void jump(uint32_t offset);

  void call(const std::string& functionName);

  void call(Ref<Object> object, int argc = 0);
  void call(Ref<Object> object, const std::vector<Ref<Object>>& args);
  void callMember(Ref<Object> self, const std::string& memberName, int argc = 0);
  void callMember(Ref<Object> self, const std::string& memberName, const std::vector<Ref<Object>>& args);
  void callFunction(Ref<Function> fn, const std::vector<Ref<Object>>& args);
  void callNativeFunction(Ref<NativeFunction> fn, const std::vector<Ref<Object>>& args);

  template <typename T>
  inline Ref<Object> popCheckType(Ref<T> type) {
    Ref<Object> value = pop();
    checkType(value, type);
    return value;
  }

  template <typename O, typename T>
  inline void checkType(Ref<O> object, Ref<T> type) {
    if (!isOfType(object, type)) {
      throw createError("TypeMismatch: expected '%s', but got '%s'", type->getTypeName().c_str(), object.template as<Instance>()->getType()->getTypeName().c_str());
    }
  }

 private:
  uint8_t current();
  uint8_t peek(int i = 0);

  CallFrame& currentFrame();
  Ref<Code>& getCode();

  RuntimeError createError(const std::string& msg);
  RuntimeError createError(const char* fmt, ...);

  void run();
  bool executeInstruction(Opcode op);

  void runCode(Ref<Code> code, std::vector<Ref<Object>> args = {});
  Ref<Object> returnCall();

  void runtimeBreakpoint();
  void printStack();
};


} /* namespace ff */

#endif /* _FF_RUNTIME_H_ */