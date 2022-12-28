#include <ff/ff.h>
#include <vector>
#include <cstdio>

using namespace ff::types;

ff::Ref<ff::Object> ff_println(ff::VM* context, std::vector<ff::Ref<ff::Object>> args) {
  printf("%s\n", args[0]->toString().c_str());
  return ff::Ref<ff::Object>();
}

ff::Ref<ff::Object> ff_printf(ff::VM* context, std::vector<ff::Ref<ff::Object>> args) {
  printf("%s\n", args[0]->toString().c_str());
  return ff::Ref<ff::Object>();
}

ff_symbol_t symbols[] {
  {"println", obj(fn(ff_println, {{"arg", any()}}, nothing()))},
  {"printf", obj(fn(ff_printf, {{"fmt", any()}}, nothing()))},
  {nullptr},
};

extern "C" ff_modinfo_t modInfo = {
  "io",
  "0.1",
  "maxrt",
  symbols
};
