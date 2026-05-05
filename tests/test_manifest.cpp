#include "catch.hpp"
#include "Manifest.hpp"
#include <fstream>
#include <filesystem>
#include <unistd.h>

namespace fs = std::filesystem;

static fs::path write_temp_manifest(const std::string& content) {
    fs::path dir = fs::temp_directory_path() / ("blackhole_test_" + std::to_string(::getpid()));
    fs::create_directories(dir);
    fs::path p = dir / "manifest.json";
    std::ofstream(p) << content;
    return p;
}

TEST_CASE("Manifest::load parses a valid recipe manifest", "[manifest]") {
    auto p = write_temp_manifest(R"({
      "schema_version": 1,
      "name": "hello",
      "version": "2.12",
      "release": 1, "epoch": 0,
      "description": "GNU hello",
      "homepage": "https://gnu.org/software/hello",
      "license": ["GPL-3.0-or-later"],
      "maintainer": {"name":"x","email":"y@z"},
      "archs": ["x86_64"], "libcs": ["glibc"],
      "sources": [
        {"url":"https://ftp.gnu.org/gnu/hello/hello-2.12.tar.gz",
         "sha256":"cf04af86dc085268c5f4470fbae49b18afbc221b78096aab842d934a76bad0ab",
         "extract": true}
      ],
      "build": {"system":"configure","options":["--prefix=/usr"],"make_jobs":"auto"},
      "depends": {"build":[],"runtime":[],"optional":[],"conflicts":[]},
      "provides": [], "replaces": [],
      "triggers": {"post_install": null, "pre_remove": null}
    })");

    Manifest m = Manifest::load(p);
    REQUIRE(m.meta.name == "hello");
    REQUIRE(m.meta.version == "2.12");
    REQUIRE(m.sources.size() == 1);
    REQUIRE(m.sources[0].url == "https://ftp.gnu.org/gnu/hello/hello-2.12.tar.gz");
    REQUIRE(m.sources[0].sha256.size() == 64);
    REQUIRE(m.sources[0].extract == true);
    REQUIRE(m.build.system == "configure");
    REQUIRE(m.build.options == std::vector<std::string>{"--prefix=/usr"});
    REQUIRE(m.build.make_jobs_auto == true);
    REQUIRE(m.recipe_dir == p.parent_path());
}

TEST_CASE("Manifest::load rejects bad sha256 length", "[manifest]") {
    auto p = write_temp_manifest(R"({
      "schema_version": 1, "name": "x", "version": "1",
      "release": 1, "epoch": 0, "description": "", "homepage": "",
      "license": [], "maintainer": {"name":"","email":""},
      "archs": ["x86_64"], "libcs": ["glibc"],
      "sources": [{"url":"https://x/y.tar.gz","sha256":"deadbeef","extract":true}],
      "build": {"system":"configure","options":[],"make_jobs":"auto"},
      "depends": {"build":[],"runtime":[],"optional":[],"conflicts":[]},
      "provides": [], "replaces": [],
      "triggers": {"post_install": null, "pre_remove": null}
    })");

    REQUIRE_THROWS_AS(Manifest::load(p), std::runtime_error);
}

TEST_CASE("Manifest::load rejects unknown build.system", "[manifest]") {
    auto p = write_temp_manifest(R"({
      "schema_version": 1, "name": "x", "version": "1",
      "release": 1, "epoch": 0, "description": "", "homepage": "",
      "license": [], "maintainer": {"name":"","email":""},
      "archs": ["x86_64"], "libcs": ["glibc"],
      "sources": [],
      "build": {"system":"alien","options":[],"make_jobs":"auto"},
      "depends": {"build":[],"runtime":[],"optional":[],"conflicts":[]},
      "provides": [], "replaces": [],
      "triggers": {"post_install": null, "pre_remove": null}
    })");

    REQUIRE_THROWS_AS(Manifest::load(p), std::runtime_error);
}

TEST_CASE("Manifest::Source honors extract_to + strip_components", "[manifest]") {
    auto p = write_temp_manifest(R"({
      "schema_version": 1, "name": "x", "version": "1",
      "release": 1, "epoch": 0, "description": "", "homepage": "",
      "license": [], "maintainer": {"name":"","email":""},
      "archs": ["x86_64"], "libcs": ["glibc"],
      "sources": [
        {"url":"https://x/a.tar.gz",
         "sha256":"0000000000000000000000000000000000000000000000000000000000000000",
         "extract": true},
        {"url":"https://x/b.tar.gz",
         "sha256":"1111111111111111111111111111111111111111111111111111111111111111",
         "extract": true,
         "extract_to": "subdir/b",
         "strip_components": 0}
      ],
      "build": {"system":"configure","options":[],"make_jobs":"auto"},
      "depends": {"build":[],"runtime":[],"optional":[],"conflicts":[]},
      "provides": [], "replaces": [],
      "triggers": {"post_install": null, "pre_remove": null}
    })");

    Manifest m = Manifest::load(p);
    REQUIRE(m.sources.size() == 2);

    // Default values when not specified.
    REQUIRE(m.sources[0].extract_to == "");
    REQUIRE(m.sources[0].strip_components == 1);

    // Explicit values round-trip.
    REQUIRE(m.sources[1].extract_to == "subdir/b");
    REQUIRE(m.sources[1].strip_components == 0);
}
