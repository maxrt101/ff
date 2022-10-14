#ifndef _FF_MODULE_H_
#define _FF_MODULE_H_ 1

#include <ff/object.h>
#include <ff/ref.h>

#define FF_MODINFO modInfo
#define FF_MODINFO_STR "modInfo"

struct ff_symbol_t {
  const char* name;
  ff::Ref<ff::Object> symbol;
};

struct ff_modinfo_t {
  const char* name;
  const char* version;
  const char* author;
  ff_symbol_t* symbols;
};

#endif /* _FF_MODULE_H_ */
