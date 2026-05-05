// builder/src/main.cpp
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

#include "CtxBuilder.hpp"
#include "Manifest.hpp"

namespace fs = std::filesystem;

static int usage(int code) {
    std::cerr << "usage: bh-builder build <recipe-dir>\n"
              << "  env: GHOST_BUILDER_WORKDIR  (default $HOME/ghost/work)\n"
              << "       GHOST_BUILDER_SOURCES  (default $HOME/ghost/sources)\n"
              << "       GHOST_BUILDER_VOIDSCRIPT (default voidscript on PATH)\n"
              << "       GHOST_BUILDER_DRIVER     (default " BLACKHOLE_BUILDER_DRIVER_VS ")\n";
    return code;
}

static fs::path env_or(const char* var, const fs::path& fallback) {
    const char* v = std::getenv(var);
    return v && *v ? fs::path(v) : fallback;
}

int main(int argc, char** argv) {
    if (argc < 3 || std::string(argv[1]) != "build") return usage(1);

    fs::path recipe_dir;
    try {
        recipe_dir = fs::canonical(argv[2]);
    } catch (const fs::filesystem_error& e) {
        std::cerr << "recipe directory not found: " << argv[2] << "\n";
        return 2;
    }

    fs::path manifest_p = recipe_dir / "manifest.json";
    fs::path recipe_vs  = recipe_dir / "recipe.vs";
    if (!fs::is_regular_file(manifest_p) || !fs::is_regular_file(recipe_vs)) {
        std::cerr << "recipe dir must contain manifest.json and recipe.vs\n";
        return 2;
    }

    fs::path home(std::getenv("HOME") ? std::getenv("HOME") : "/tmp");
    fs::path workdir = env_or("GHOST_BUILDER_WORKDIR",  home / "ghost" / "work");
    fs::path sources = env_or("GHOST_BUILDER_SOURCES",  home / "ghost" / "sources");
    fs::path driver  = env_or("GHOST_BUILDER_DRIVER",   BLACKHOLE_BUILDER_DRIVER_VS);
    std::string voidscript_bin = std::getenv("GHOST_BUILDER_VOIDSCRIPT")
        ? std::getenv("GHOST_BUILDER_VOIDSCRIPT") : "voidscript";

    fs::create_directories(workdir);
    fs::create_directories(sources);

    Manifest m;
    try { m = Manifest::load(manifest_p); }
    catch (const std::exception& e) {
        std::cerr << "manifest error: " << e.what() << "\n";
        return 3;
    }

    // Layer-0 only ships x86_64-glibc; cross-arch comes in Layer 1 wrapper.
    CtxBuilder cb;
    cb.set_workdir_root(workdir);
    cb.set_sources_cache(sources);
    cb.set_recipe_vs(recipe_vs);
    cb.set_arch_libc("x86_64", "glibc");

    nlohmann::json ctx = cb.build(m);

    fs::path build_root = workdir / (m.meta.name + "-" + m.meta.version + "-" +
                                     std::to_string(m.meta.release));
    fs::create_directories(build_root);
    fs::path ctx_path = build_root / "ctx.json";
    {
        std::ofstream out(ctx_path);
        out << ctx.dump(2);
    }
    std::cout << "wrote " << ctx_path << "\n";

    // Exec voidscript driver.vs <ctx-path>
    if (!fs::is_regular_file(driver)) {
        std::cerr << "driver.vs not found at " << driver
                  << " (set GHOST_BUILDER_DRIVER)\n";
        return 4;
    }

    std::string ctx_str    = ctx_path.string();
    std::string driver_str = driver.string();
    char* args[] = {
        const_cast<char*>(voidscript_bin.c_str()),
        const_cast<char*>(driver_str.c_str()),
        const_cast<char*>(ctx_str.c_str()),
        nullptr
    };
    ::execvp(args[0], args);
    perror("execvp voidscript");
    return 127;
}
