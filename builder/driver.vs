// builder/driver.vs — bh-builder voidscript driver
// Invoked as:  voidscript driver.vs <ctx.json>
//
// Phases:
//   1. Read ctx.json
//   2. Download every source[] into sources_cache via curl, verify sha256
//   3. Extract first source into srcdir (Layer-0: one tarball, extract=true)
//   4. Concat recipe.vs with a dispatcher epilogue, run via voidscript subprocess
//   5. Walk pkgdir → write FILES.sha256
//   6. Pack stage/ into <name>-<ver>-<rel>.<arch>.blackhole.unsigned  (tar.zst)
//
// API notes:
//   - No curl_get_to_file: shell out to system curl for binary downloads.
//   - No file_mkdir_p: use mkdir(path, true) for recursive mkdir.
//   - No foreach: use  for (type $var : iterable) { }
//   - archive_create does not recurse into dirs: shell out to tar for packing.
//   - process_run argv preserves array order (voidscript fix b9e7ac8): pass
//     arguments as ["arg0", "arg1", ...] arrays — no shell-string concat,
//     no injection risk from build paths.
//   - find | sort | sha256sum is a genuine shell pipeline; kept as `sh -c`.

if ($argc < 2) {
    print("usage: voidscript driver.vs <ctx.json>\n");
    exit(1);
}

string $ctx_path = $argv[1];
object $ctx      = json_decode(file_get_contents($ctx_path));

string $name           = $ctx["manifest"]["name"];
string $version        = $ctx["manifest"]["version"];
string $release        = $ctx["manifest"]["release"];
string $arch           = $ctx["arch"];
string $srcdir         = $ctx["srcdir"];
string $builddir       = $ctx["builddir"];
string $pkgdir         = $ctx["pkgdir"];
string $sources_cache  = $ctx["sources_cache"];
string $recipe_vs_path = $ctx["recipe_vs"];
string $workdir        = path_dirname($srcdir);

print("[driver] " + $name + "-" + $version + "-" + $release + " on " + $arch + "\n");

// --- Reset workdirs ---
file_rm_rf($srcdir);
file_rm_rf($builddir);
file_rm_rf($pkgdir);
mkdir($srcdir,        true);
mkdir($builddir,      true);
mkdir($pkgdir,        true);
mkdir($sources_cache, true);

// -----------------------------------------------------------------------
// Phase 1: Download + verify sha256 + extract sources
// -----------------------------------------------------------------------
for (object $src : $ctx["sources"]) {
    string $url        = $src["url"];
    string $sha        = $src["sha256"];
    string $file_name  = path_basename($url);
    string $cache_path = path_join($sources_cache, $sha + "-" + $file_name);

    if (!path_is_file($cache_path)) {
        print("[driver] downloading " + $url + "\n");
        object $dl = process_run("curl",
            ["-fsSL", "--output", $cache_path, $url],
            { boolean env_clear: false });
        if ($dl["exit_code"] != 0) {
            print("[driver] download failed for: " + $url + "\n");
            print($dl["stderr"]);
            exit(10);
        }
    }

    string $got = hash_file("sha256", $cache_path);
    if (!hash_compare($got, $sha)) {
        print("[driver] sha256 mismatch for " + $url + "\n");
        print("  expected: " + $sha + "\n");
        print("  got:      " + $got + "\n");
        exit(11);
    }
    print("[driver] verified " + $file_name + "\n");

    if ($src["extract"]) {
        archive_extract($cache_path, $srcdir, {
            int strip_components: 1
        });
        print("[driver] extracted " + $file_name + " -> " + $srcdir + "\n");
    }
}

// -----------------------------------------------------------------------
// Phase 2: Run recipe
// Concatenate recipe.vs with a dispatcher epilogue, then run combined
// script via a voidscript subprocess.
// -----------------------------------------------------------------------
// "voidscript" is expected on PATH (installed system-wide on the build VM,
// or via the GHOST_BUILDER_VOIDSCRIPT env var honored by bh-builder).
string $voidscript_bin = "voidscript";
string $recipe_src     = file_get_contents($recipe_vs_path);

// The dispatcher re-reads ctx.json so the recipe can access $ctx.
// function_exists() is a builtin (ModuleHelperModule).
string $dispatcher = "\n\n// ---- bh-builder dispatcher (auto-generated) ----\n"
    + "object $ctx = json_decode(file_get_contents(\"" + $ctx_path + "\"));\n"
    + "if (function_exists(\"prepare\")) { prepare($ctx); }\n"
    + "if (function_exists(\"build\"))   { build($ctx);   }\n"
    + "if (function_exists(\"check\"))   { check($ctx);   }\n"
    + "if (function_exists(\"package\")) { package($ctx); }\n";

string $combined_path = path_join($workdir, "combined_recipe.vs");
file_put_contents($combined_path, $recipe_src + $dispatcher, true);

print("[driver] running recipe\n");
object $res = process_run($voidscript_bin,
    [$combined_path],
    { boolean env_clear: false });

if ($res["exit_code"] != 0) {
    print("[driver] recipe failed (exit " + $res["exit_code"] + ")\n");
    print($res["stdout"]);
    print($res["stderr"]);
    exit(20);
}
print($res["stdout"]);

// -----------------------------------------------------------------------
// Phase 3: Walk pkgdir → FILES.sha256
// Use sha256sum over all files found via find+sort for determinism.
// Output format: "<hex>  <rel_path>\n"
// -----------------------------------------------------------------------
print("[driver] building FILES.sha256\n");

object $sha_res = process_run("sh",
    ["-c",
     "LC_ALL=C find " + $pkgdir + " -type f | LC_ALL=C sort | xargs -r sha256sum | sed 's|  " + $pkgdir + "/|  |'"],
    { boolean env_clear: false });

if ($sha_res["exit_code"] != 0) {
    print("[driver] sha256 walk failed\n");
    print($sha_res["stderr"]);
    exit(23);
}

string $files_sha = $sha_res["stdout"];

// Count staged files
object $count_res = process_run("sh",
    ["-c", "find " + $pkgdir + " -type f | wc -l | tr -d ' \\n'"],
    { boolean env_clear: false });

int $count = 0;
if ($count_res["exit_code"] == 0) {
    $count = string_to_number($count_res["stdout"]);
}

print("[driver] " + $count + " files staged\n");

// -----------------------------------------------------------------------
// Phase 4: Build META.json with build_provenance
// -----------------------------------------------------------------------
object $meta = $ctx["manifest"];
$meta["build_provenance"] = {
    string builder_version:    "blackhole 0.3.0",
    string voidscript_version: "0.1.1",
    string build_started_at:   $ctx_path,
    int    file_count:         $count
};

// -----------------------------------------------------------------------
// Phase 5: Stage then pack .blackhole.unsigned (tar.zst, deterministic)
//
// archive_create does not recurse into subdirectories, so use system tar.
// Layout inside the archive:
//   META.json
//   FILES.sha256
//   payload/      (full pkgdir tree)
// -----------------------------------------------------------------------
string $out_basename = $name + "-" + $version + "-" + $release + "." + $arch + ".blackhole.unsigned";
string $out_path     = path_join($workdir, $out_basename);
string $stage        = path_join($workdir, "stage");

file_rm_rf($stage);
mkdir($stage, true);

file_put_contents(path_join($stage, "META.json"),    json_encode($meta), true);
file_put_contents(path_join($stage, "FILES.sha256"), $files_sha,         true);

// Copy pkgdir tree into stage/payload/
file_copy($pkgdir, path_join($stage, "payload"), { boolean recursive: true });

// Pack with tar --zstd, reproducible flags.
object $tar_res = process_run("tar",
    ["--zstd", "-cf", $out_path,
     "-C", $stage,
     "--mtime=@0", "--owner=0", "--group=0", "--numeric-owner", "--sort=name",
     "META.json", "FILES.sha256", "payload"],
    { boolean env_clear: false });

if ($tar_res["exit_code"] != 0) {
    print("[driver] tar packing failed\n");
    print($tar_res["stderr"]);
    exit(30);
}

// Clean up intermediates we no longer need (combined recipe + stage tree).
file_rm_rf($combined_path);
file_rm_rf($stage);

print("[driver] wrote " + $out_path + "\n");
exit(0);
