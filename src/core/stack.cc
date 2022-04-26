#include <ff/stack.h>

const char* ff::StackOverflowException::what() const noexcept {
  return "Stack Overflow Occured";
}

const char* ff::StackUnderflowException::what() const noexcept {
  return "Stack Underflow Occured";
}
