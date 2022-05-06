#include <ff/utils/path.h>
#include <mrt/strutils.h>
#include <sys/stat.h>
#include <unistd.h>

#define _MAX_PATH 1024

bool ff::path::isRoot(const std::string& path) {
  return path.size() && path[0] == '/';
}

bool ff::path::exists(const std::string& path) {
  struct stat buffer;   
  return stat(path.c_str(), &buffer) == 0;
}

std::string ff::path::concat(std::string lhs, std::string rhs) {
  if (!lhs.size()) return rhs;
  if (!rhs.size()) return lhs;

  if (lhs.back() != '/') {
    lhs.push_back('/');
  }
  if (rhs.front() == '/') {
    lhs.erase(lhs.begin());
  }

  return lhs + rhs;
}

std::string ff::path::getcwd() {
  static char cwd[_MAX_PATH];
  if (::getcwd(cwd, sizeof(cwd)) != 0) {
    return std::string(cwd);
  }
  return "";
}

std::string ff::path::getFullPath(const std::string& path) {
  if (isRoot(path)) return path;
  return concat(getcwd(), path);
}

std::string ff::path::getFolder(std::string path) {
  size_t pos = path.rfind('/');
  if (pos != std::string::npos) {
    return path.substr(0, pos);
  }
  return path;
}

std::string ff::path::getFile(std::string path) {
  size_t pos = path.rfind('/');
  if (pos != std::string::npos) {
    return path.substr(pos+1);
  }
  return path;
}

std::string ff::path::getExtension(std::string path) {
  size_t pos = path.rfind('.');
  if (pos != std::string::npos) {
    return path.substr(pos+1);
  }
  return path;
}

std::string ff::path::stripExtension(std::string file) {
  size_t pos = file.rfind('.');
  if (pos != std::string::npos) {
    return file.substr(0, pos);
  }
  return file;
}

std::string ff::path::getImportFile(const std::string& file) {
  if (mrt::str::endsWith(file, ".ff") || mrt::str::endsWith(file, ".ffmod")) {
    return file;
  }

  if (exists(file)) {
    return file;
  }

  if (exists(file + ".ff")) {
    return file + ".ff";
  }

  if (exists(file + ".ffmod")) {
    return file + ".ffmod";
  }

  return "";
}

std::string ff::path::getImportFileFromPath(const std::string& file, std::vector<std::string> paths) {
  for (auto& folder : paths) {
    std::string result = getImportFile(concat(folder, file));
    if (result != "") {
      return result;
    }
  }
  return file;
}
