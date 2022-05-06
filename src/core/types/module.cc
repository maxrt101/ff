#include <ff/types/module.h>
#include <ff/types/string.h>
#include <ff/compiler/compiler.h>
#include <ff/compiler/scanner.h>
#include <ff/compiler/parser.h>
#include <ff/runtime.h>
#include <ff/config.h>
#include <ff/memory.h>
#include <ff/errors.h>
#include <ff/types.h>
#include <ff/ast.h>
#include <iostream>
#include <fstream>

ff::Ref<ff::ModuleType> ff::ModuleType::m_instance = nullptr;

ff::ModuleType::ModuleType() : Type("module") {
  setField("__as_string__", NativeFunction::createInstance([](VM* context, std::vector<Ref<Object>> args) {
    return String::createInstance("<module" + args[0].asRefTo<Module>()->name + ">").template asRefTo<Object>();
  }, {
    {"self", TypeAnnotation::create("module")}
  }, TypeAnnotation::create("string")).asRefTo<Object>());
}

ff::ModuleType::~ModuleType() {}

std::string ff::ModuleType::toString() const {
  return "module";
}

ff::Ref<ff::ModuleType> ff::ModuleType::getInstance() {
  if (!m_instance.get()) {
    m_instance = memory::allocate<ModuleType>();
    new (m_instance.get()) ModuleType();
  }
  return m_instance;
}

ff::Module::Module(const std::string& name) : Instance(ModuleType::getInstance().asRefTo<Type>()), name(name) {}

ff::Module::~Module() {}

std::string ff::Module::toString() const {
  // std::string result = "{";
  // for (int i = 0; i < value.size(); i++) {
  //   result += value[i]->toString();
  //   if (i + 1 < value.size()) result += ", ";
  // }
  // return result + "}";
  return "<module " + name + ">";
}

bool ff::Module::equals(Ref<Object> other) const {
  return other->getObjectType() == OTYPE_INSTANCE
      && other.as<Instance>()->getType() == getType();
      // && other.as<Module>()->value == value;
}

ff::Ref<ff::Module> ff::Module::createInstance(const std::string& name) {
  return memory::construct<Module>(name);
}

ff::Compiler::ModuleInfo ff::loadModule(const std::string& name, const std::string& filename) {
  Ref<Module> result = Module::createInstance(name);

  std::ifstream sourceFile(filename);
  if (!sourceFile) {
    throw CompileError(filename, -1, "Error opening file");
  }
  std::string src((std::istreambuf_iterator<char>(sourceFile)), std::istreambuf_iterator<char>());

  ff::Scanner scanner(filename, src);
  auto tokens = scanner.tokenize();
  ff::Parser parser(filename, tokens);
  auto tree = parser.parse();
  if (config::get("debug") != "0") {
    ast::printTree(tree);
  }
  ff::Compiler compiler;
  ff::Ref<ff::Code> code = compiler.compile(filename, tree);
  if (config::get("debug") != "0") {
    printf("=== Code ===\n\\\n");
    ast::unwrapCode(code);
  }
  ff::VM vm;
  vm.run(code);

  auto& globals = vm.getGlobals();
  if (globals.find(name) == globals.end()) {
    throw CompileError(filename, 1, "Cannot find module '%s'", name.c_str());
  }

  auto& fields = globals[name]->getFields();

  for (auto& field : fields) {
    result->setField(field.first, field.second);
  }

  return {result, compiler.getGlobals()[name]};
}

