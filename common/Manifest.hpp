#ifndef _BLACKHOLE_MANIFEST_HPP_
#define _BLACKHOLE_MANIFEST_HPP_

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

#include "PackageMetaV1.hpp"

struct Source {
    std::string url;
    std::string sha256;
    bool extract = true;
    std::string extract_to;          // relative path inside $srcdir; "" = root
    int strip_components = 1;        // tar --strip-components; 0 to disable
};

inline void from_json(const nlohmann::json& j, Source& s) {
    j.at("url").get_to(s.url);
    j.at("sha256").get_to(s.sha256);
    if (j.contains("extract"))           j.at("extract").get_to(s.extract);
    if (j.contains("extract_to"))        j.at("extract_to").get_to(s.extract_to);
    if (j.contains("strip_components"))  j.at("strip_components").get_to(s.strip_components);
    static const std::regex hex64("^[0-9a-fA-F]{64}$");
    if (!std::regex_match(s.sha256, hex64)) {
        throw std::runtime_error("source sha256 not 64-char hex: " + s.sha256);
    }
    if (s.strip_components < 0) {
        throw std::runtime_error("source strip_components must be >= 0, got " +
                                 std::to_string(s.strip_components));
    }
}

struct BuildSpec {
    std::string system;                     // configure | cmake | meson | make | custom
    std::vector<std::string> options;
    bool make_jobs_auto = true;
    int  make_jobs = 0;                      // valid only when !make_jobs_auto
};

inline void from_json(const nlohmann::json& j, BuildSpec& b) {
    j.at("system").get_to(b.system);
    static const std::vector<std::string> allowed = {"configure","cmake","meson","make","custom"};
    if (std::find(allowed.begin(), allowed.end(), b.system) == allowed.end()) {
        throw std::runtime_error("build.system not in {configure,cmake,meson,make,custom}: " + b.system);
    }
    if (j.contains("options")) j.at("options").get_to(b.options);
    if (j.contains("make_jobs")) {
        const auto& mj = j.at("make_jobs");
        if (mj.is_string() && mj.get<std::string>() == "auto") {
            b.make_jobs_auto = true;
        } else if (mj.is_number_integer()) {
            b.make_jobs_auto = false;
            b.make_jobs = mj.get<int>();
        } else {
            throw std::runtime_error("build.make_jobs must be \"auto\" or integer, got: " + mj.dump());
        }
    }
}

struct Manifest {
    PackageMetaV1 meta;
    std::vector<Source> sources;
    BuildSpec build;
    std::filesystem::path recipe_dir;       // populated by load(), not from JSON

    static Manifest load(const std::filesystem::path& manifest_json) {
        std::ifstream in(manifest_json);
        if (!in) throw std::runtime_error("cannot open " + manifest_json.string());
        nlohmann::json j;
        in >> j;
        Manifest m;
        m.meta = j.get<PackageMetaV1>();
        if (j.contains("sources")) j.at("sources").get_to(m.sources);
        m.build = j.at("build").get<BuildSpec>();
        m.recipe_dir = manifest_json.parent_path();
        return m;
    }
};

#endif
