#ifndef _FF_TYPES_FLOAT_H_
#define _FF_TYPES_FLOAT_H_ 1

#include <ff/object.h>
#include <ff/ref.h>

namespace ff {

class FloatType : public Type {
 private:
  static Ref<FloatType> m_instance;

  FloatType();

 public:
  ~FloatType();

  std::string toString() const override;

  static Ref<FloatType> getInstance();
};

class Float : public Instance {
 public:
  using ValueType = double;

  ValueType value;

 public:
  explicit Float(ValueType value = 0.0);
  ~Float();

  std::string toString() const override;
  bool equals(Ref<Object> other) const override;

  static Ref<Float> createInstance(ValueType value = 0.0);
};

} /* namespace ff */

#endif /* _FF_TYPES_FLOAT_H_ */