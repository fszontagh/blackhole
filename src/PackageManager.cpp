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
std::vector<int> PackageManager::splitVersion(const std::string& ver) {
    std::vector<int> parts;
    std::istringstream iss(ver);
    std::string token;
    while (std::getline(iss, token, '.')) {
        // std::stoi kivételt dob, ha hibás a szám, de itt feltételezzük, hogy jól formázott
        parts.push_back(std::stoi(token));
    }
    return parts;
}
int PackageManager::compareVersions(const std::string& ver1, const std::string& ver2) {
    auto v1 = splitVersion(ver1);
    auto v2 = splitVersion(ver2);

    // Hosszabbik verzióhoz igazítjuk a rövidebbet 0 értékekkel.
    size_t len = std::max(v1.size(), v2.size());
    v1.resize(len, 0);
    v2.resize(len, 0);

    for (size_t i = 0; i < len; ++i) {
        if (v1[i] < v2[i])
            return -1;
        else if (v1[i] > v2[i])
            return 1;
    }
    return 0;
}
bool PackageManager::validatePackage(const std::string& fullSpec, const std::string& pkgName, const std::string& pkgVersion) {
    std::istringstream ss(fullSpec);
    std::string packageGroup;
    bool packageFound = false;
    bool overallValid = true;

    while (std::getline(ss, packageGroup, ',')) {
        // Először eltávolítjuk a kezdő és végző whitespace-eket.
        packageGroup = std::regex_replace(packageGroup, std::regex("^\\s+|\\s+$"), "");

        // Az OR ágak felosztása (ha van | karakter)
        std::vector<std::string> alternatives;
        std::istringstream issAlt(packageGroup);
        std::string alt;
        while (std::getline(issAlt, alt, '|')) {
            // Trim whitespace
            alt = std::regex_replace(alt, std::regex("^\\s+|\\s+$"), "");
            alternatives.push_back(alt);
        }

        bool groupValid   = false;
        bool clauseForPkg = false;

        std::regex clauseRe(R"(^\s*([A-Za-z0-9_.-]+)(?:\s*(>=|==|>|<)\s*([0-9]+(?:\.[0-9]+){0,2}))?\s*$)");
        for (const auto& altClause : alternatives) {
            std::smatch match;
            if (std::regex_match(altClause, match, clauseRe)) {
                std::string clauseName = match[1];
                std::string op         = match[2];
                std::string ver        = match[3];

                if (clauseName != pkgName)
                    continue;

                clauseForPkg = true;
                if (op.empty()) {
                    groupValid = true;
                    break;
                }

                int cmp          = compareVersions(pkgVersion, ver);
                bool clauseValid = false;
                if (op == "==")
                    clauseValid = (cmp == 0);
                else if (op == ">")
                    clauseValid = (cmp == 1);
                else if (op == "<")
                    clauseValid = (cmp == -1);
                else if (op == ">=")
                    clauseValid = (cmp == 0 || cmp == 1);

                if (clauseValid) {
                    groupValid = true;
                    break;
                }
            }
        }

        if (clauseForPkg && !groupValid) {
            overallValid = false;
            break;
        }
    }

    if (!overallValid)
        return false;
    else
        return true;
}
