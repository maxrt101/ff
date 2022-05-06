#ifndef _FF_TYPES_VECTOR_H_
#define _FF_TYPES_VECTOR_H_ 1

#include <ff/object.h>
#include <ff/ref.h>
#include <string>
#include <vector>

namespace ff {

class VectorType : public Type {
 private:
  static Ref<VectorType> m_instance;

  VectorType();

 public:
  ~VectorType();

  std::string toString() const override;

  static Ref<VectorType> getInstance();
};

class Vector : public Instance {
 public:
  using ValueType = std::vector<Ref<Object>>;

  ValueType value;

 public:
  explicit Vector(const ValueType& value);
  ~Vector();

  std::string toString() const override;
  bool equals(Ref<Object> other) const override;

  static Ref<Vector> createInstance(const ValueType& value);
};

} /* namespace ff */

#endif /* _FF_TYPES_VECTOR_H_ */