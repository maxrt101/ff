#ifndef _FF_REF_H_
#define _FF_REF_H_ 1

#include <cstddef>
#include <cstdlib>

#ifdef _FF_REF_DEBUG
#include <iostream>
#include <mrt/functions.h>
#endif

namespace ff {

namespace memory::raw {
template <typename T> inline void free(T* ptr);
} /* namespace memory::raw */

template <typename T>
class Ref {
 public:
  using SizeType = size_t; 

 private:
  T* m_data = nullptr;
  SizeType* m_count = nullptr;

 public:
  inline Ref() {}

  inline Ref(T* ptr) : m_data(ptr), m_count(new SizeType{0}) {
#ifdef _FF_REF_DEBUG
    std::cout << "Ref<" << mrt::getTypeName<T>() << ">(" << m_data << ") ptr (count " << m_count << ")\n";
#endif
    (*m_count)++;
  }

  inline Ref(const Ref& rhs) {
    *this = rhs;
  }

  inline Ref(Ref&& rhs) {
    *this = std::move(rhs);
  }

  inline Ref& operator=(T* ptr) {
    cleanup();
    m_data = ptr;
    m_count = new SizeType;
    (*m_count)++;
    return *this;
  }

  inline Ref& operator=(const Ref& rhs) {
    cleanup();
    if (rhs.m_data) {
      m_data = rhs.m_data;
      m_count = rhs.m_count;
      (*m_count)++;
    }
#ifdef _FF_REF_DEBUG
    std::cout << "Ref<" << mrt::getTypeName<T>() <<  ">(" << m_data << ") copy (count=" << (int)(m_count ? *m_count : -1) << ")\n";
#endif
    return *this;
  }

  inline Ref& operator=(Ref&& rhs) {
    cleanup();
    m_data = rhs.m_data;
    m_count = rhs.m_count;
    rhs.m_data = nullptr;
    rhs.m_count = nullptr;
#ifdef _FF_REF_DEBUG
    std::cout << "Ref<" << mrt::getTypeName<T>() << ">(" << m_data << ") move (&count=" << m_count << ", cpunt" << (int)(m_count ? *m_count : -1) << ")\n";
#endif
    return *this;
  }

  inline ~Ref() {
    cleanup();
  }

  inline T* operator->() const {
    return m_data;
  }

  inline T& operator*() const {
    return *m_data;
  }

  inline bool operator==(const Ref& rhs) const {
    return m_data == rhs.m_data;
  }

  inline bool operator!=(const Ref& rhs) const {
    return m_data != rhs.m_data;
  }

  inline T* get() const {
    return m_data;
  }

  template <typename K>
  inline K* as() const {
    return (K*)m_data;
  }

  template <typename R>
  inline Ref<R> asRefTo() {
#ifdef _FF_REF_DEBUG
    std::cout << "Ref<" << mrt::getTypeName<T>() << ">(" << m_data << ")::asRefTo<" << mrt::getTypeName<R>() << ">() &count=" << m_count <<  " count=" << (int)(m_count ? *m_count : -1) << "\n";
#endif
    Ref<R> ref;
    ref.m_count = m_count;
    ref.m_data = (R*)m_data;
    (*m_count)++;
    return ref;
  }

  inline SizeType count() const {
    return *m_count;
  }

  inline void reset() {
    cleanup();
  }

 private:
  void cleanup() {
#ifdef _FF_REF_DEBUG
    std::cout << "Ref<" << mrt::getTypeName<T>() << ">(" << m_data << ")::cleanup() &count=" << m_count <<  "\n";
    // std::cout << "Ref<" << mrt::getTypeName<T>() << ">(" << m_data << ")::cleanup() &count=" << m_count << " count=" << (int)(m_count ? *m_count : -1) << "\n";
#endif
    if (m_data && m_count) {
      (*m_count)--;
      if (*m_count <= 0) {
#ifdef _FF_REF_DEBUG
        std::cout << "Ref<" << mrt::getTypeName<T>() << ">(" << m_data << ")::cleanup() delete count=" << m_count <<  "\n";
#endif
        delete m_count;
        if (m_data) {
#ifdef _FF_REF_DEBUG
          std::cout << "Ref<" << mrt::getTypeName<T>() << ">(" << m_data << ")::cleanup() delete\n";
#endif
          memory::raw::free(m_data);
        }
        m_data = nullptr;
        m_count = nullptr;
      }
    }
  }

  template <typename>
  friend class Ref;

};

} /* namespace ff */

#endif /* _FF_REF_H_ */