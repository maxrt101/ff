#include <ff/config.h>
#include <mrt/container_utils.h>

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
