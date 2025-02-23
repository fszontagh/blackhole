#include "PackageManager.hpp"

PackageManager::~PackageManager() {}

void PackageManager::installPackage(const std::string& packageName) {}

bool PackageManager::loadPackageMeta(const std::string& jsonFile) {
    std::ifstream file(jsonFile);
    if (!file) {
        this->logger->error("Cannot open meta file: " + jsonFile);
        return false;
    }
    try {
        file >> metaData_;
    } catch (const std::exception& e) {
        this->logger->error("Error parsing JSON: " + std::string(e.what()));
        return false;
    }
    return true;
}
std::vector<PackageMeta>
PackageManager::searchPackages(const std::string& query) {
    return std::vector<PackageMeta>();
};