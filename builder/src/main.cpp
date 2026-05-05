// builder/src/main.cpp
#include <filesystem>
#include <iostream>
#include <string>

#include "Manifest.hpp"

namespace fs = std::filesystem;

static int usage(int code) {
    std::cerr << "usage: bh-builder build <recipe-dir>\n";
    return code;
}

int main(int argc, char** argv) {
    if (argc < 2) return usage(1);
    std::string cmd = argv[1];
    if (cmd != "build") return usage(1);
    if (argc < 3) return usage(1);

    fs::path recipe_dir;
    try {
        recipe_dir = fs::canonical(argv[2]);
    } catch (const fs::filesystem_error& e) {
        std::cerr << "recipe directory not found: " << argv[2] << "\n";
        return 2;
    }
    fs::path manifest_path = recipe_dir / "manifest.json";
    fs::path recipe_vs     = recipe_dir / "recipe.vs";

    if (!fs::is_regular_file(manifest_path)) {
        std::cerr << "no manifest.json in " << recipe_dir << "\n";
        return 2;
    }
    if (!fs::is_regular_file(recipe_vs)) {
        std::cerr << "no recipe.vs in " << recipe_dir << "\n";
        return 2;
    }

    try {
        Manifest m = Manifest::load(manifest_path);
        std::cout << "loaded recipe " << m.meta.name << "-" << m.meta.version
                  << "-" << m.meta.release << " (" << m.sources.size()
                  << " source(s))\n";
    } catch (const std::exception& e) {
        std::cerr << "manifest error: " << e.what() << "\n";
        return 3;
    }
    return 0;
}
