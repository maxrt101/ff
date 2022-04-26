#ifndef _FF_CONFIG_H_
#define _FF_CONFIG_H_ 1

#include <string>
#include <vector>
#include <map>

namespace ff {
namespace config {

void initialize();
bool exists(const std::string& key);
std::string get(const std::string& key);
void set(const std::string& key, const std::string& value);
std::map<std::string, std::string>& getAll();
std::vector<std::string> getKeys();

} /* namespace config */
} /* namespace ff */

#endif /* _FF_CONFIG_H_ */
