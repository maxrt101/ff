#ifndef _FF_TYPES_CPTR_H_
#define _FF_TYPES_CPTR_H_ 1

#include <ff/object.h>
#include <ff/ref.h>

namespace ff {

class CPtrType : public Type {
 private:
  static Ref<CPtrType> m_instance;

  CPtrType();

 public:
  ~CPtrType();

  std::string toString() const override;

  static Ref<CPtrType> getInstance();
};

class CPtr : public Instance {
 public:
  using ValueType = void*;

  ValueType value;

 public:
  explicit CPtr(ValueType value = nullptr);
  ~CPtr();

  std::string toString() const override;
  bool equals(Ref<Object> other) const override;

  static Ref<CPtr> createInstance(ValueType value = nullptr);
};

} /* namespace ff */

#endif /* _FF_TYPES_CPTR_H_ */