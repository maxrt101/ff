#ifndef _FF_TYPES_STRING_H_
#define _FF_TYPES_STRING_H_ 1

#include <ff/object.h>
#include <ff/ref.h>
#include <string>

namespace ff {

class StringType : public Type {
 private:
  static Ref<StringType> m_instance;

  StringType();

 public:
  ~StringType();

  std::string toString() const override;

  static Ref<StringType> getInstance();
};

class String : public Instance {
 public:
  using ValueType = std::string;

  ValueType value;

 public:
  explicit String(const ValueType& value = "");
  ~String();

  std::string toString() const override;
  bool equals(Ref<Object> other) const override;

  static Ref<String> createInstance(const ValueType& value = "");
  static Ref<String> createInstancePool(const ValueType& value = "");
};

} /* namespace ff */

#endif /* _FF_TYPES_STRING_H_ */