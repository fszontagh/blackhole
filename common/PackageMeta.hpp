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

enum class PackageDependencyType { OPTIONAL,
                                   REQUIREMENT,
                                   CONFLICT,
                                   UNKNOWN };

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
struct PackageMetaDependency {
    std::string name;
    std::string version;
    PackageDependencyEquality equality;
    PackageDependencyType type;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PackageMetaDependency, name, version, equality, type)
};

struct PackageMeta {
    std::string name;
    std::vector<PackageMetaDependency> dependencies;
    std::vector<std::string> files;
    std::string description;
    PackageMetaVersion version;
    PackageMetaAuthor author;
    int creationTime;
    std::string checksum;  // checksum is a sha256 hash

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PackageMeta, name, dependencies, files, description, version, author, creationTime, checksum)
};

struct RemotePackageIndex {
    std::vector<PackageMeta> packages;
    std::string url;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(RemotePackageIndex, url, packages)
};

#endif  // _BLACKHOLE_PACKAGEMETA_HPP_