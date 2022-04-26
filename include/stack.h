#ifndef _FF_STACK_H_
#define _FF_STACK_H_ 1

#include <mrt/memory.h>
#include <ff/memory.h>
#include <exception>
#include <vector>
#include <cstddef>

namespace ff {

class StackOverflowException : public std::exception {
 public:
  StackOverflowException() = default;

  const char* what() const noexcept override;
};

class StackUnderflowException : public std::exception {
 public:
  StackUnderflowException() = default;

  const char* what() const noexcept override;
};

template <typename T>
class Stack {
 private:
  std::vector<T> m_data;

 public:
  inline Stack() {
    reset();
  }

  inline ~Stack() {}

  inline void reset() {
    // m_stackTop = m_stack;
    m_data.clear();
  }

  inline size_t size() const {
    return m_data.size();
  }

  inline bool canPop() const {
    return m_data.size() > 0;
  }

  inline void push(T value) {
    m_data.push_back(value);
  }

  inline T pop() {
    if (!canPop()) {
      throw StackUnderflowException();
    }
    T object = m_data.back();
    m_data.pop_back();
    return object;
  }

  inline T& peek(int distance = 0) {
    return m_data[m_data.size()-1-distance];
  }

  inline T& operator [](unsigned index) {
    return m_data[index];
  }

  inline std::vector<T>& getBuffer() {
    return m_data;
  }
};

} /* namespace ff */

#endif /* _FF_STACK_H_ */