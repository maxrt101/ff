#include <ff/utils/dynamic_library_manager.h>

std::unordered_map<std::string, ff::Ref<mrt::DynamicLibrary>> ff::DynamicLibraryManager::m_libraries;

void ff::DynamicLibraryManager::setLibrary(const std::string& name, Ref<mrt::DynamicLibrary> library) {
  m_libraries[name] = library;
}

bool ff::DynamicLibraryManager::libraryExists(const std::string& name) {
  return m_libraries.find(name) != m_libraries.end();
}

ff::Ref<mrt::DynamicLibrary> ff::DynamicLibraryManager::getLibrary(const std::string& name) {
  return m_libraries[name];
}

void ff::DynamicLibraryManager::deleteLibrary(const std::string& name) {
  m_libraries.erase(name);
}

void ff::DynamicLibraryManager::clear() {
  m_libraries.clear();
}
