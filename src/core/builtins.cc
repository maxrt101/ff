#include <ff/builtins.h>
#include <ff/runtime.h>
#include <ff/errors.h>
#include <ff/types.h>

using namespace ff::types;

ff::Ref<ff::NativeFunction> ff::fn_exit = fn(
  [](ff::VM* context, std::vector<ff::Ref<ff::Object>> args) {
    context->stop();
    context->setReturnCode(intval(args[0]));
    return ff::Ref<ff::Object>();
  },
  {{"retCode", type("int")}},
  nothing()
);

ff::Ref<ff::NativeFunction> ff::fn_assert = fn(
  [](ff::VM* context, std::vector<ff::Ref<ff::Object>> args) {
    if (!ff::Object::toBool(context, args[0])) {
      throw ff::RuntimeError::createf("Assertion Failed");
    }
    return ff::Ref<ff::Object>();
  },
  {{"value", type("bool")}},
  nothing()
);


