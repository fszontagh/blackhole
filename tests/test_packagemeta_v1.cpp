#include "catch.hpp"
#include "PackageMetaV1.hpp"
#include <nlohmann/json.hpp>

TEST_CASE("PackageMetaV1 round-trips minimal valid metadata", "[packagemeta_v1]") {
    nlohmann::json j = {
        {"schema_version", 1},
        {"name", "zlib"},
        {"version", "1.3.1"},
        {"release", 1},
        {"epoch", 0},
        {"description", "compression"},
        {"homepage", "https://zlib.net"},
        {"license", {"Zlib"}},
        {"maintainer", {{"name","x"},{"email","y@z"}}},
        {"archs", {"x86_64"}},
        {"libcs", {"glibc"}},
        {"depends", {{"build", nlohmann::json::array()},
                     {"runtime", nlohmann::json::array()},
                     {"optional", nlohmann::json::array()},
                     {"conflicts", nlohmann::json::array()}}},
        {"provides", nlohmann::json::array()},
        {"replaces", nlohmann::json::array()},
        {"triggers", {{"post_install", nullptr}, {"pre_remove", nullptr}}}
    };

    PackageMetaV1 m = j.get<PackageMetaV1>();
    REQUIRE(m.schema_version == 1);
    REQUIRE(m.name == "zlib");
    REQUIRE(m.version == "1.3.1");
    REQUIRE(m.release == 1);
    REQUIRE(m.epoch == 0);
    REQUIRE(m.archs == std::vector<std::string>{"x86_64"});
    REQUIRE(m.libcs == std::vector<std::string>{"glibc"});

    nlohmann::json round = m;
    REQUIRE(round["name"] == "zlib");
    REQUIRE(round["archs"] == std::vector<std::string>{"x86_64"});
}

TEST_CASE("PackageMetaV1 rejects schema_version != 1", "[packagemeta_v1]") {
    nlohmann::json j = {
        {"schema_version", 2}, {"name", "x"}, {"version", "1"},
        {"release", 1}, {"epoch", 0}, {"description", ""},
        {"homepage", ""}, {"license", nlohmann::json::array()},
        {"maintainer", {{"name",""},{"email",""}}},
        {"archs", nlohmann::json::array()}, {"libcs", nlohmann::json::array()},
        {"depends", {{"build", nlohmann::json::array()},
                     {"runtime", nlohmann::json::array()},
                     {"optional", nlohmann::json::array()},
                     {"conflicts", nlohmann::json::array()}}},
        {"provides", nlohmann::json::array()}, {"replaces", nlohmann::json::array()},
        {"triggers", {{"post_install", nullptr},{"pre_remove", nullptr}}}
    };

    REQUIRE_THROWS_AS(j.get<PackageMetaV1>(), std::runtime_error);
}

TEST_CASE("PackageMetaV1 rejects shell-metachar name/version", "[packagemeta_v1]") {
    auto base = nlohmann::json{
        {"schema_version", 1}, {"name", "ok"}, {"version", "1"},
        {"release", 1}, {"epoch", 0}, {"description", ""},
        {"homepage", ""}, {"license", nlohmann::json::array()},
        {"maintainer", {{"name",""},{"email",""}}},
        {"archs", nlohmann::json::array()}, {"libcs", nlohmann::json::array()},
        {"depends", {{"build", nlohmann::json::array()},
                     {"runtime", nlohmann::json::array()},
                     {"optional", nlohmann::json::array()},
                     {"conflicts", nlohmann::json::array()}}},
        {"provides", nlohmann::json::array()}, {"replaces", nlohmann::json::array()},
        {"triggers", {{"post_install", nullptr},{"pre_remove", nullptr}}}
    };

    SECTION("name with shell metas") {
        auto j = base; j["name"] = "evil; rm -rf /";
        REQUIRE_THROWS_AS(j.get<PackageMetaV1>(), std::runtime_error);
    }
    SECTION("name with space") {
        auto j = base; j["name"] = "two words";
        REQUIRE_THROWS_AS(j.get<PackageMetaV1>(), std::runtime_error);
    }
    SECTION("version with backticks") {
        auto j = base; j["version"] = "1.0`whoami`";
        REQUIRE_THROWS_AS(j.get<PackageMetaV1>(), std::runtime_error);
    }
    SECTION("typical safe values pass") {
        auto j = base;
        j["name"] = "lib-foo+bar.baz";
        j["version"] = "1.2.3+rc4-1";
        REQUIRE_NOTHROW(j.get<PackageMetaV1>());
    }
}
