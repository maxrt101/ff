#ifndef _FF_BUILTINS_H_
#define _FF_BUILTINS_H_ 1

#include <ff/ref.h>
#include <ff/object.h>
#include <ff/types/native_function.h>
#include <vector>

namespace ff {

extern Ref<NativeFunction> fn_exit;
extern Ref<NativeFunction> fn_assert;
extern Ref<NativeFunction> fn_type;
extern Ref<NativeFunction> fn_inspect;
extern Ref<NativeFunction> fn_memaddr;

} /* namespace ff */

#endif /* _FF_BUILTINS_H_ */