#ifndef _FF_UTILS_STRUTILS_H_
#define _FF_UTILS_STRUTILS_H_ 1

#include <vector>
#include <string>

namespace ff {
namespace str {

std::string repeat(const std::string& s, int count);
std::string join(const std::vector<std::string>& vec, const std::string& sep = "");
int toInt(const std::string& str);

} /* namespace str */
} /* namespace ff */

#endif /* _FF_UTILS_STRUTILS_H_ */