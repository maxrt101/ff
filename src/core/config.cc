#include <ff/config.h>
#include <ff/log.h>
#include <mrt/container_utils.h>
#include <cstdlib>

static std::map<std::string, std::string> g_values;

void ff::config::initialize() {
  set("entry", "main");
  set("debug", "0");
}

bool ff::config::exists(const std::string& key) {
  return g_values.find(key) != g_values.end();
}

std::string ff::config::get(const std::string& key) {
  return g_values.at(key);
}

void ff::config::set(const std::string& key, const std::string& value) {
  g_values[key] = value;
}

std::map<std::string, std::string>& ff::config::getAll() {
  return g_values;
}

std::vector<std::string> ff::config::getKeys() {
  return mrt::reduce<std::vector<std::string>>(g_values, [](auto keys, auto pair) {
    keys.push_back(pair.first);
    return keys;
  });
}

std::string ff::config::format(const std::string& value) {
  std::string result;

  size_t i = 0;
  while (i < value.size()) {
    if (value[i] == '{') {
      std::string key;
      i++;
      while (i < value.size() && value[i] != '}') {
        key += value[i++];
      }
      if (i >= value.size()) {
        error("config.format failed: unterminated bracket");
        exit(1);
      }
      i++;
      result += get(key);
    } else {
      result += value[i++];
    }
  }

  return result;
}
