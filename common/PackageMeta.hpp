#ifndef _BLACKHOLE_PACKAGEMETA_HPP_
#define _BLACKHOLE_PACKAGEMETA_HPP_
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

enum class PackageDependencyEquality {
    EQUAL,
    GREATER_THAN,
    LESS_THAN,
    GREATER_THAN_EQUAL,
    LESS_THAN_EQUAL
};

const std::unordered_map<std::string, PackageDependencyEquality> equalityMap = {{"=", PackageDependencyEquality::EQUAL},
                                                                                {">", PackageDependencyEquality::GREATER_THAN},
                                                                                {"<", PackageDependencyEquality::LESS_THAN},
                                                                                {">=", PackageDependencyEquality::GREATER_THAN_EQUAL},
                                                                                {"<=", PackageDependencyEquality::LESS_THAN_EQUAL}};

inline static PackageDependencyEquality equality(const std::string& s) {
    return equalityMap.at(s);
}
inline static std::string equality(PackageDependencyEquality e) {
    for (const auto& p : equalityMap) {
        if (p.second == e) {
            return p.first;
        }
    }
    return "";
}

enum class PackageDependencyType { OPTIONAL,
                                   REQUIREMENT,
                                   CONFLICT,
                                   UNKNOWN };

const std::unordered_map<std::string, PackageDependencyType> typeMap = {{"opt", PackageDependencyType::OPTIONAL},
                                                                        {"req", PackageDependencyType::REQUIREMENT},
                                                                        {"conf", PackageDependencyType::CONFLICT},
                                                                        {"unk", PackageDependencyType::UNKNOWN}};

inline static PackageDependencyType type(const std::string& s) {
    return typeMap.at(s);
}

inline static std::string type(PackageDependencyType t) {
    for (const auto& p : typeMap) {
        if (p.second == t) {
            return p.first;
        }
    }
    return "";
}

struct PackageMetaVersion {
    std::string major;
    std::string minor;
    std::string patch;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PackageMetaVersion, major, minor, patch)
};

struct PackageMetaAuthor {
    std::string name;
    std::string email;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PackageMetaAuthor, name, email)
};


struct PackageMeta {
    std::string name;
    std::vector<std::string> files;
    std::string description;
    PackageMetaVersion version;
    PackageMetaAuthor author;
    int creationTime;
    std::string checksum;                 // checksum is a sha256 hash
    std::string optional_string    = "";  // optional packages in string format
    std::string requirement_string = "";  // requirement packages in string format
    std::string conflict_string    = "";  // conflict packages in string format

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PackageMeta, name, files, description, version, author, creationTime, checksum, optional_string, requirement_string, conflict_string)
};

struct RemotePackageIndex {
    std::vector<PackageMeta> packages;
    std::string url;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(RemotePackageIndex, url, packages)
};

#endif  // _BLACKHOLE_PACKAGEMETA_HPP_