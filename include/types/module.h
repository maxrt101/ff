#ifndef _FF_TYPES_MODULE_H_
#define _FF_TYPES_MODULE_H_ 1

#include <ff/object.h>
#include <ff/ref.h>
#include <string>

namespace ff {

class ModuleType : public Type {
 private:
  static Ref<ModuleType> m_instance;

  ModuleType();

 public:
  ~ModuleType();

  std::string toString() const override;

  static Ref<ModuleType> getInstance();
};

class Module : public Instance {
 public:
  std::string name;

 public:
  Module(const std::string& name);
  ~Module();

  std::string toString() const override;
  bool equals(Ref<Object> other) const override;

  static Ref<Module> createInstance(const std::string& name);
};

} /* namespace ff */

#endif /* _FF_TYPES_MODULE_H_ */