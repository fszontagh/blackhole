// builder/src/CtxBuilder.cpp
#include "CtxBuilder.hpp"
#include <thread>

namespace fs = std::filesystem;

nlohmann::json CtxBuilder::build(const Manifest& m) const {
    int jobs = m.build.make_jobs_auto
        ? std::max(1u, std::thread::hardware_concurrency())
        : m.build.make_jobs;

    fs::path workdir = workdir_root_ /
        (m.meta.name + "-" + m.meta.version + "-" + std::to_string(m.meta.release));

    nlohmann::json ctx;
    ctx["manifest"]      = m.meta;                          // serializes via PackageMetaV1
    ctx["srcdir"]        = (workdir / "srcdir").string();
    ctx["builddir"]      = (workdir / "builddir").string();
    ctx["pkgdir"]        = (workdir / "pkgdir").string();
    ctx["sources_cache"] = sources_cache_.string();
    ctx["recipe_dir"]    = m.recipe_dir.string();
    ctx["recipe_vs"]     = recipe_vs_.string();
    ctx["jobs"]          = jobs;
    ctx["arch"]          = arch_;
    ctx["libc"]          = libc_;
    ctx["cross_target"]  = arch_ + "-pc-ghostlinux-gnu";
    ctx["options"]       = m.build.options;
    ctx["build_system"]  = m.build.system;

    nlohmann::json sources = nlohmann::json::array();
    for (const auto& s : m.sources) {
        sources.push_back({
            {"url", s.url},
            {"sha256", s.sha256},
            {"extract", s.extract},
            {"extract_to", s.extract_to},
            {"strip_components", s.strip_components}
        });
    }
    ctx["sources"] = sources;

    return ctx;
}
