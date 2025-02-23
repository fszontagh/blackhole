#ifndef _BLACKHOLE_PACKAGEDATABASE_HPP_
#define _BLACKHOLE_PACKAGEDATABASE_HPP_

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class PackageDatabase {
private:
  std::string installedFile;
  std::string repoFile;
  json installedData;
  json repoData;

public:
  PackageDatabase(const std::string &installedPath, const std::string &repoPath)
      : installedFile(installedPath), repoFile(repoPath) {
    loadInstalledData();
    loadRepoData();
  }

  void loadInstalledData() {
    std::ifstream file(installedFile);
    if (file.is_open()) {
      file >> installedData;
      file.close();
    } else {
      installedData = json{{"installed_packages", json::object()}};
    }
  }

  void loadRepoData() {
    std::ifstream file(repoFile);
    if (file.is_open()) {
      file >> repoData;
      file.close();
    } else {
      repoData = json{{"packages", json::object()}};
    }
  }

  void saveInstalledData() {
    std::ofstream file(installedFile);
    if (file.is_open()) {
      file << installedData.dump(4);
      file.close();
    }
  }

  void saveRepoData() {
    std::ofstream file(repoFile);
    if (file.is_open()) {
      file << repoData.dump(4);
      file.close();
    }
  }

  void addInstalledPackage(const std::string &name, const std::string &version,
                           const std::vector<std::string> &files,
                           const std::vector<std::string> &dependencies) {
    installedData["installed_packages"][name] = {
        {"version", version}, {"files", files}, {"dependencies", dependencies}};
    saveInstalledData();
  }

  void removeInstalledPackage(const std::string &name) {
    installedData["installed_packages"].erase(name);
    saveInstalledData();
  }

  bool isPackageInstalled(const std::string &name) {
    return installedData["installed_packages"].contains(name);
  }

  json getInstalledPackages() { return installedData["installed_packages"]; }

  json getAvailablePackages() { return repoData["packages"]; }
};

#endif // _BLACKHOLE_PACKAGEDATABASE_HPP_