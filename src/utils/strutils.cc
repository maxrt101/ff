#include <ff/strutils.h>

std::string ff::str::repeat(const std::string& s, int count) {
  std::string result;
  for (int i = 0; i < count; i++) {
    result += s;
  }
  return result;
}
