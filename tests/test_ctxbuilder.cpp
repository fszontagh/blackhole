#include "catch.hpp"
#include "CtxBuilder.hpp"
#include <unistd.h>

TEST_CASE("CtxBuilder produces complete ctx for a manifest", "[ctxbuilder]") {
    Manifest m;
    m.meta.name = "hello";
    m.meta.version = "2.12";
    m.meta.release = 1;
    m.meta.archs = {"x86_64"};
    m.meta.libcs = {"glibc"};
    m.build.system = "configure";
    m.build.options = {"--prefix=/usr"};
    m.build.make_jobs_auto = true;
    m.recipe_dir = "/data/ghostlinux/recipes/sys-test/hello";

    CtxBuilder cb;
    cb.set_workdir_root("/tmp/gh-work");
    cb.set_sources_cache("/tmp/gh-sources");
    cb.set_recipe_vs(m.recipe_dir / "recipe.vs");
    cb.set_arch_libc("x86_64", "glibc");

    nlohmann::json ctx = cb.build(m);

    REQUIRE(ctx["manifest"]["name"] == "hello");
    REQUIRE(ctx["srcdir"]   == "/tmp/gh-work/hello-2.12-1/srcdir");
    REQUIRE(ctx["builddir"] == "/tmp/gh-work/hello-2.12-1/builddir");
    REQUIRE(ctx["pkgdir"]   == "/tmp/gh-work/hello-2.12-1/pkgdir");
    REQUIRE(ctx["sources_cache"] == "/tmp/gh-sources");
    REQUIRE(ctx["recipe_dir"]   == "/data/ghostlinux/recipes/sys-test/hello");
    REQUIRE(ctx["recipe_vs"]    == "/data/ghostlinux/recipes/sys-test/hello/recipe.vs");
    REQUIRE(ctx["jobs"].get<int>() >= 1);
    REQUIRE(ctx["arch"]   == "x86_64");
    REQUIRE(ctx["libc"]   == "glibc");
    REQUIRE(ctx["cross_target"] == "x86_64-pc-ghostlinux-gnu");
    REQUIRE(ctx["options"] == std::vector<std::string>{"--prefix=/usr"});
}

TEST_CASE("CtxBuilder honors make_jobs override", "[ctxbuilder]") {
    Manifest m;
    m.meta.name = "x"; m.meta.version = "1"; m.meta.release = 1;
    m.meta.archs = {"x86_64"}; m.meta.libcs = {"glibc"};
    m.build.system = "configure";
    m.build.make_jobs_auto = false;
    m.build.make_jobs = 3;
    m.recipe_dir = "/tmp";

    CtxBuilder cb;
    cb.set_workdir_root("/tmp/w");
    cb.set_sources_cache("/tmp/s");
    cb.set_recipe_vs("/tmp/recipe.vs");
    cb.set_arch_libc("x86_64", "glibc");

    nlohmann::json ctx = cb.build(m);
    REQUIRE(ctx["jobs"] == 3);
}
