#ifndef _FF_TYPES_CALLABLE_H_
#define _FF_TYPES_CALLABLE_H_ 1

#include <ff/object.h>

namespace ff {

class Callable {
 public:
  virtual Ref<Object> call(VM*, int nargs);
};

} /* namespace ff */

#endif /* _FF_TYPES_CALLABLE_H_ */