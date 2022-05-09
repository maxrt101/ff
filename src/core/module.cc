#include <ff/module.h>
#include <ff/object.h>
#include <ff/types/string.h>
#include <ff/compiler/compiler.h>
#include <ff/compiler/type_annotation.h>
#include <mrt/dynamic_library.h>

ff::Compiler::ModuleInfo ff::loadNativeModule(const std::string& name, const std::string& filename) {
  Ref<Module> module = Module::createInstance(name);
  std::map<std::string, ASTAnnotation> annotations;

  auto lib = Ref(new mrt::DynamicLibrary(filename));

  ff_modinfo_t* modInfo = lib->getSymbolAs<ff_modinfo_t*>(FF_MODINFO_STR);

  if (!modInfo) {
    throw CompileError(filename, -1, "Cannot find module info symbol ('%s') in module %s", FF_MODINFO_STR, name.c_str());
  }

  module->setField("__author__", String::createInstance(modInfo->author).asRefTo<Object>());
  module->setField("__version__", String::createInstance(modInfo->version).asRefTo<Object>());

  for (int i = 0; modInfo->symbols[i].name; i++) {
    module->setField(modInfo->symbols[i].name, modInfo->symbols[i].symbol);
  }

  for (int i = 0; modInfo->annotations[i].name; i++) {
    annotations[modInfo->annotations[i].name] = modInfo->annotations[i].annotation;
  }

  Compiler::ModuleInfo result = {
    name,
    module,
    Compiler::Variable::fromObject(name, module.asRefTo<Object>()),
    {},
    {{name, lib}},
    annotations};

  return result;
}
