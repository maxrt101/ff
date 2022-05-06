#ifndef _FF_TYPES_INT_H_
#define _FF_TYPES_INT_H_ 1

#include <ff/object.h>
#include <ff/ref.h>

namespace ff {

class IntType : public Type {
 private:
  static Ref<IntType> m_instance;

  IntType();

 public:
  ~IntType();

  std::string toString() const override;

  static Ref<IntType> getInstance();
};

class Int : public Instance {
 public:
  using ValueType = int64_t;

  ValueType value;

 public:
  explicit Int(ValueType value = 0);
  ~Int();

  std::string toString() const override;
  bool equals(Ref<Object> other) const override;

  static Ref<Int> createInstance(ValueType value = 0);
};

} /* namespace ff */

#endif /* _FF_TYPES_INT_H_ */