#ifndef _FF_UTILS_PATHUTILS_H_
#define _FF_UTILS_PATHUTILS_H_ 1

#include <vector>
#include <string>

namespace ff {
namespace path {

bool isRoot(const std::string& path);
bool exists(const std::string& path);
std::string concat(std::string lhs, std::string rhs);
std::string getcwd();
std::string getFullPath(const std::string& path);
std::string getFolder(std::string path);
std::string getFile(std::string path);
std::string stripExtension(std::string file);
std::string getExtension(std::string path);
std::string getImportFile(const std::string& file);
std::string getImportFileFromPath(const std::string& file, std::vector<std::string> paths);

} /* namespace path */
} /* namespace ff */

#endif /* _FF_UTILS_PATHUTILS_H_ */