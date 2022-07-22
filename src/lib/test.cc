#include <ff/ff.h>
#include <vector>
#include <cstdio>

using namespace ff::types;

ff::Ref<ff::Object> Test_test(ff::VM* context, std::vector<ff::Ref<ff::Object>> args) {
  printf("Test.test: %s %s\n", args[1]->toString().c_str(), args[2]->toString().c_str());
  return ff::Ref<ff::Object>();
}

ff_symbol_t symbols[] {
  {"Test", obj(
    classobj(
        "Test",
        {
          {"counter", ff::Class::Field {"counter", false, obj(integer(10))}}
        },
        {
          {"test", obj(fn(Test_test, {{"self", any()}, {"arg1", any()}, {"arg2", any()}}, nothing()))}
        }
      )
    )
  },
  {nullptr},
};

extern "C" ff_modinfo_t modInfo = {
  "test",
  "0.1",
  "maxrt",
  symbols
};
