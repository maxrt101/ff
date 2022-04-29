#ifndef _FF_OBJECT_H_
#define _FF_OBJECT_H_ 1

#include <cstdint>
#include <string>
#include <map>
#include <ff/ref.h>

namespace ff {

class VM;

enum ObjectType {
  OTYPE_INSTANCE,
  OTYPE_TYPE,
};

class Object {
 private:
  ObjectType m_type;
  std::map<std::string, Ref<Object>> m_fields;

 public:
  Object(ObjectType type);
  virtual ~Object() = default;

  ObjectType getObjectType() const;
  bool isInstance() const;
  bool isType() const;

  bool hasField(const std::string& key) const;
  Ref<Object> getField(const std::string& key);
  void setField(const std::string& key, Ref<Object> value);
  std::map<std::string, Ref<Object>>& getFields();

  virtual std::string toString() const;
  virtual bool equals(Ref<Object> other) const;

  static Ref<Object> cast(VM* context, Ref<Object> object, const std::string& typeName);
};

class Type : public Object {
 private:
  std::string m_typeName;

 public:
  Type(const std::string& typeName);
  virtual ~Type() = default;

  std::string getTypeName() const;

  bool equals(Ref<Object> other) const override;
};

class Instance : public Object {
 private:
  Ref<Type> m_type;

 public:
  Instance(Ref<Type> type);
  virtual ~Instance() = default;

  Ref<Type> getType() const;
};

class NullType : public Type {
 private:
  static Ref<NullType> m_instance;

  NullType();

 public:
  ~NullType();

  std::string toString() const override;

  static Ref<NullType> getInstance();
};

} /* namespace ff */

#endif /* _FF_OBJECT_H_ */