#ifndef _BLACKHOLE_PACKAGEMETA_V1_HPP_
#define _BLACKHOLE_PACKAGEMETA_V1_HPP_

#include <stdexcept>
#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

struct MaintainerV1 {
    std::string name;
    std::string email;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MaintainerV1, name, email)
};

struct DependsV1 {
    std::vector<std::string> build;
    std::vector<std::string> runtime;
    std::vector<std::string> optional;
    std::vector<std::string> conflicts;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DependsV1, build, runtime, optional, conflicts)
};

struct TriggersV1 {
    std::optional<std::string> post_install;
    std::optional<std::string> pre_remove;
};

inline void to_json(nlohmann::json& j, const TriggersV1& t) {
    j["post_install"] = t.post_install ? nlohmann::json(*t.post_install) : nlohmann::json(nullptr);
    j["pre_remove"]   = t.pre_remove   ? nlohmann::json(*t.pre_remove)   : nlohmann::json(nullptr);
}
inline void from_json(const nlohmann::json& j, TriggersV1& t) {
    if (j.at("post_install").is_null()) t.post_install.reset();
    else t.post_install = j.at("post_install").get<std::string>();
    if (j.at("pre_remove").is_null()) t.pre_remove.reset();
    else t.pre_remove = j.at("pre_remove").get<std::string>();
}

struct PackageMetaV1 {
    int schema_version = 1;
    std::string name;
    std::string version;
    int release = 1;
    int epoch = 0;
    std::string description;
    std::string homepage;
    std::vector<std::string> license;
    MaintainerV1 maintainer;
    std::vector<std::string> archs;
    std::vector<std::string> libcs;
    DependsV1 depends;
    std::vector<std::string> provides;
    std::vector<std::string> replaces;
    TriggersV1 triggers;
};

inline void to_json(nlohmann::json& j, const PackageMetaV1& m) {
    j = nlohmann::json{
        {"schema_version", m.schema_version},
        {"name", m.name}, {"version", m.version},
        {"release", m.release}, {"epoch", m.epoch},
        {"description", m.description}, {"homepage", m.homepage},
        {"license", m.license}, {"maintainer", m.maintainer},
        {"archs", m.archs}, {"libcs", m.libcs},
        {"depends", m.depends},
        {"provides", m.provides}, {"replaces", m.replaces},
        {"triggers", m.triggers}
    };
}

inline void from_json(const nlohmann::json& j, PackageMetaV1& m) {
    j.at("schema_version").get_to(m.schema_version);
    if (m.schema_version != 1) {
        throw std::runtime_error("PackageMetaV1: unsupported schema_version " +
                                 std::to_string(m.schema_version));
    }
    j.at("name").get_to(m.name);
    j.at("version").get_to(m.version);
    j.at("release").get_to(m.release);
    j.at("epoch").get_to(m.epoch);
    j.at("description").get_to(m.description);
    j.at("homepage").get_to(m.homepage);
    j.at("license").get_to(m.license);
    j.at("maintainer").get_to(m.maintainer);
    j.at("archs").get_to(m.archs);
    j.at("libcs").get_to(m.libcs);
    j.at("depends").get_to(m.depends);
    j.at("provides").get_to(m.provides);
    j.at("replaces").get_to(m.replaces);
    j.at("triggers").get_to(m.triggers);
}

#endif
