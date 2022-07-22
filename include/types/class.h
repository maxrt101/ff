#ifndef _FF_TYPES_CLASS_H_
#define _FF_TYPES_CLASS_H_ 1

#include <ff/compiler/type_annotation.h>
#include <ff/object.h>
#include <ff/ref.h>
#include <unordered_map>

namespace ff {

class ClassType : public Type {
 private:
  static Ref<ClassType> m_instance;

  ClassType();

 public:
  ~ClassType();

  std::string toString() const override;

  static Ref<ClassType> getInstance();
};

class Class : public Instance {
 public:
  struct Field {
    std::string name;
    bool isStatic = false;
    Ref<Object> initialValue;
  };

  std::string className;
  std::unordered_map<std::string, Field> fieldInfo;

 public:
  explicit Class(const std::string& className);
  explicit Class(const std::string& className, const std::unordered_map<std::string, Field>& fieldInfo, const std::unordered_map<std::string, Ref<Object>>& methods);
  ~Class();

  std::string toString() const override;
  bool equals(Ref<Object> other) const override;

  static Ref<Class> createInstance(const std::string& className);
  static Ref<Class> createInstance(const std::string& className, const std::unordered_map<std::string, Field>& fieldInfo, const std::unordered_map<std::string, Ref<Object>>& methods);
};

class ClassInstanceType : public Type {
 private:
  static Ref<ClassInstanceType> m_instance;

  ClassInstanceType();

 public:
  ~ClassInstanceType();

  std::string toString() const override;

  static Ref<ClassInstanceType> getInstance();
};

class ClassInstance : public Instance {
 private:
  Ref<Class> m_class;

 public:
  explicit ClassInstance(Ref<Class> class_);
  ~ClassInstance();

  Ref<Class> getClass() const;

  std::string toString() const override;
  bool equals(Ref<Object> other) const override;

  static Ref<ClassInstance> createInstance(Ref<Class> class_); // Ref<Vector> constructorParameters // Ref<Dict>
};

} /* namespace ff */

#endif /* _FF_TYPES_CLASS_H_ */