#ifndef _FF_TYPES_DICT_H_
#define _FF_TYPES_DICT_H_ 1

#include <ff/object.h>
#include <ff/ref.h>
#include <string>
#include <map>

namespace ff {

class DictType : public Type {
 private:
  static Ref<DictType> m_instance;

  DictType();

 public:
  ~DictType();

  std::string toString() const override;

  static Ref<DictType> getInstance();
};

class Dict : public Instance {
 public:
  using ValueType = std::map<std::string, Ref<Object>>;

 public:
  Dict(ValueType value);
  ~Dict();

  std::string toString() const override;
  bool equals(Ref<Object> other) const override;

  static Ref<Dict> createInstance(ValueType value);
};

} /* namespace ff */

#endif /* _FF_TYPES_DICT_H_ */