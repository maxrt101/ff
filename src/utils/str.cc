#include <ff/utils/str.h>

std::string ff::str::repeat(const std::string& s, int count) {
  std::string result;
  for (int i = 0; i < count; i++) {
    result += s;
  }
  return result;
}

std::string ff::str::join(std::vector<std::string> vec, std::string sep) {
  std::string result;
  for (int i = 0; i < vec.size(); i++) {
    result += vec[i];
    if (i + 1 < vec.size()) result += sep;
  }
  return result;
}

int ff::str::toInt(const std::string& str) {
  int base = 10;
  if (str.size() > 2 && str[1] == 'b') base = 2;
  if (str.size() > 2 && str[1] == 'x') base = 16;
  return std::stoi(base != 10 ? str.substr(2) : str, nullptr, base);
}
