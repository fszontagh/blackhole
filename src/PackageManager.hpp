#ifndef PACKAGE_MANAGER_HPP
#define PACKAGE_MANAGER_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "Logger.hpp"
#include "PackageMeta.hpp"

class PackageManager {
public:
  PackageManager(std::shared_ptr<Logger> logger) : logger(logger) {};
  ~PackageManager();

  void installPackage(const std::string &packageName);
  bool loadPackageMeta(const std::string &jsonFile);
  void updateRemotePackageLists();
  std::vector<PackageMeta> searchPackages(const std::string &query);

private:
  std::shared_ptr<Logger> logger;
  nlohmann::json metaData_;

  std::vector<int> splitVersion(const std::string& ver);
  int compareVersions(const std::string& ver1, const std::string& ver2);
  bool validatePackage(const std::string& fullSpec, const std::string& pkgName, const std::string& pkgVersion);
};

#endif // PACKAGE_MANAGER_HPP
