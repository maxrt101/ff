#ifndef _FF_UTILS_DYNAMIC_LIBRARY_MANAGER_H_
#define _FF_UTILS_DYNAMIC_LIBRARY_MANAGER_H_ 1

#include <mrt/dynamic_library.h>
#include <ff/ref.h>
#include <unordered_map>
#include <string>
#include <vector>

namespace ff {

class DynamicLibraryManager {
 private:
  static std::unordered_map<std::string, Ref<mrt::DynamicLibrary>> m_libraries;

 private:
  DynamicLibraryManager() = default;

 public:
  ~DynamicLibraryManager() = default;

  static void setLibrary(const std::string& name, Ref<mrt::DynamicLibrary> library);
  static bool libraryExists(const std::string& name);
  static Ref<mrt::DynamicLibrary> getLibrary(const std::string& name);
  static void deleteLibrary(const std::string& name);
  static void clear();
};

} /* namespace ff */

#endif /* _FF_UTILS_DYNAMIC_LIBRARY_MANAGER_H_ */