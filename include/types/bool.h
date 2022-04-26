#ifndef _FF_TYPES_BOOL_H_
#define _FF_TYPES_BOOL_H_ 1

#include <ff/object.h>
#include <ff/ref.h>

namespace ff {

class BoolType : public Type {
 private:
  static Ref<BoolType> m_instance;

  BoolType();

 public:
  ~BoolType();

  std::string toString() const override;

  static Ref<BoolType> getInstance();
};

class Bool : public Instance {
 public:
  using ValueType = bool;

  ValueType value;

 public:
  Bool(ValueType value = false);
  ~Bool();

  std::string toString() const override;
  bool equals(Ref<Object> other) const override;

  static Ref<Bool> createInstance(ValueType value = false);

  static Ref<Bool> getTrue();
  static Ref<Bool> getFalse();
};

} /* namespace ff */

#endif /* _FF_TYPES_BOOL_H_ */