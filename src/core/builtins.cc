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

ff::Ref<ff::NativeFunction> ff::fn_type = fn(
  [](ff::VM* context, std::vector<ff::Ref<ff::Object>> args) {
    if (!args[0].get()) {
      return obj(string("null"));
    }
    if (args[0]->isInstance()) {
      if (args[0].as<ff::Instance>()->getType() == ff::ClassInstanceType::getInstance().asRefTo<Type>()) {
        return obj(string(args[0].as<ff::ClassInstance>()->getClass()->className));
      } else {
        return obj(string(args[0].as<ff::Instance>()->getType()->getTypeName()));
      }
    } else {
      return obj(string("type"));
    }
  },
  {{"value", any()}},
  type("string")
);


ff::Ref<ff::NativeFunction> ff::fn_inspect = fn(
  [](ff::VM* context, std::vector<ff::Ref<ff::Object>> args) {
    if (!args[0].get()) {
      printf("null");
      return ff::Ref<ff::Object>();
    }
    if (args[0]->isInstance()) {
      if (args[0].as<ff::Instance>()->getType() == ff::ClassInstanceType::getInstance().asRefTo<Type>()) {
        printf("ClassInstance: %s\n", args[0].as<ff::ClassInstance>()->getClass()->className.c_str());
        for (auto& [fieldName, field] : args[0].as<ff::ClassInstance>()->getFields()) {
          printf("  %s: %s\n", fieldName.c_str(), field->toString().c_str());
        }
      } else {
        printf("Instance: %s\n", args[0].as<ff::Instance>()->getType()->getTypeName().c_str());
        for (auto& [fieldName, field] : args[0].as<ff::ClassInstance>()->getFields()) {
          printf("  %s: %s\n", fieldName.c_str(), field->toString().c_str());
        }
      }
    } else {
      printf("Type: %s\n", args[0].as<ff::Type>()->getTypeName().c_str());
      for (auto& [fieldName, field] : args[0].as<ff::ClassInstance>()->getFields()) {
          printf("  %s: %s\n", fieldName.c_str(), field->toString().c_str());
        }
    }
    return ff::Ref<ff::Object>();
  },
  {{"value", any()}},
  nothing()
);

ff::Ref<ff::NativeFunction> ff::fn_memaddr = fn(
  [](ff::VM* context, std::vector<ff::Ref<ff::Object>> args) {
    if (!args[0].get()) {
      return obj(string("null"));
    }
    char buf[32] = {0};
    snprintf(buf, 32, "0x%p", (void*)args[0].get());
    return obj(string(std::string(buf)));
  },
  {{"value", any()}},
  type("string")
);
