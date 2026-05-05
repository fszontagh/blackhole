// builder/src/CtxBuilder.hpp
#ifndef _BLACKHOLE_BUILDER_CTXBUILDER_HPP_
#define _BLACKHOLE_BUILDER_CTXBUILDER_HPP_

#include <filesystem>
#include <nlohmann/json.hpp>

#include "Manifest.hpp"

class CtxBuilder {
  public:
    void set_workdir_root(std::filesystem::path p) { workdir_root_ = std::move(p); }
    void set_sources_cache(std::filesystem::path p) { sources_cache_ = std::move(p); }
    void set_recipe_vs(std::filesystem::path p) { recipe_vs_ = std::move(p); }
    void set_arch_libc(std::string arch, std::string libc) {
        arch_ = std::move(arch); libc_ = std::move(libc);
    }

    nlohmann::json build(const Manifest& m) const;

  private:
    std::filesystem::path workdir_root_;
    std::filesystem::path sources_cache_;
    std::filesystem::path recipe_vs_;
    std::string arch_, libc_;
};

#endif
