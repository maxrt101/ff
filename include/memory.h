#ifndef _FF_MEMORY_H_
#define _FF_MEMORY_H_ 1

#include <cstdlib>
#include <ff/ref.h>

#ifdef _FF_MEMORY_DEBUG
#include <mrt/functions.h>
#include <iostream>
#endif

namespace ff {
namespace memory {
namespace raw {

template <typename T>
inline void free(T* ptr) {
#ifdef _FF_MEMORY_DEBUG
  std::cout << "memory::raw::free<" << mrt::getTypeName<T>() << ">(" << ptr << ")" << std::endl;
#endif
  ptr->~T();
  ::free(ptr);
}

template <typename T>
inline T* reallocate(T* ptr, size_t count = 1) {
#ifdef _FF_MEMORY_DEBUG
  std::cout << "memory::raw::reallocate<" << mrt::getTypeName<T>() << ">(" << ptr << ", " << count << ")" << std::endl;
#endif

  if (ptr && count == 0) {
    free(ptr);
    return nullptr;
  }

  void* result = realloc((void*)ptr, count * sizeof(T));

#ifdef _FF_MEMORY_DEBUG
  if (!ptr) {
    std::cout << "memory::raw::reallocate<" << mrt::getTypeName<T>() << "> NEW (ptr=" << result << ", sizeof:" << sizeof(T) << ", count:" << count << ")" << std::endl;
  } else {
    std::cout << "memory::raw::reallocate<" << mrt::getTypeName<T>() << "> REALLOC (" << ptr << "->" << result << ", sizeof:" << sizeof(T) << ", count:" << count << ")" << std::endl;
  }
#endif

  if (!result) {
    // TODO ERROR Allocation Failed
    exit(-1);
  }

  return (T*)result;
}

template <typename T>
inline T* allocate(size_t count = 1) {
  return reallocate<T>(nullptr, count);
}

template <typename T, typename... Args>
inline T* construct(Args&&... args) {
  T* ptr = allocate<T>(1);
  new (ptr) T(args...);
  return ptr;
}

} /* namespace raw */

template <typename T>
inline Ref<T> allocate(size_t count = 1) {
  return Ref(raw::allocate<T>(count));
}

template <typename T, typename... Args>
inline Ref<T> construct(Args&&... args) {
  return Ref(raw::construct<T>(args...));
}

} /* namespace memory */
} /* namespace ff */

#endif /* _FF_CORE_MEMORY_H_ */