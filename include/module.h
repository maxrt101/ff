#ifndef _FF_MODULE_H_
#define _FF_MODULE_H_ 1

#include <ff/compiler/type_annotation.h>
#include <ff/compiler/ast_annotation.h>
#include <ff/types/native_function.h>
#include <ff/object.h>
#include <ff/ref.h>

#define FF_MODINFO modInfo
#define FF_MODINFO_STR "modInfo"

using TFunction = ff::Object*(*)(ff::VM*, std::vector<ff::Ref<ff::Object>>);

struct ff_symbol_t {
  const char* name;
  ff::Ref<ff::Object> symbol;
};

struct ff_annotation_t {
  const char* name;
  ff::ASTAnnotation annotation;
};

struct ff_modinfo_t {
  const char* name;
  const char* version;
  const char* author;
  ff_symbol_t* symbols;
  ff_annotation_t* annotations;
};

#endif /* _FF_MODULE_H_ */
