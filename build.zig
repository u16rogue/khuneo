const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const is_debug = optimize == .Debug;

    // --------
    //  Khuneo
    // --------
    const module_khuneo = b.addModule("khuneo", .{
        .root_source_file = b.path("khuneo/src/khuneo.zig"),
        .target = target,
        .optimize = optimize,
    });

    // ----------------
    //  Khuneo Targets
    // ----------------
    if (b.option(bool, "KHUNEO_LIBRARY_STATIC", "Make the khuneo static library available. default: release false, debug true") orelse is_debug) {
        const target_khuneo_static = b.addStaticLibrary(.{
            .name = "khuneo-static",
            .root_source_file = b.path("libkhuneo/src/khuneo.zig"),
            .target = target,
            .optimize = optimize,
            .link_libc = true,
        });
        target_khuneo_static.root_module.addImport("khuneo", module_khuneo);
        b.installArtifact(target_khuneo_static);
        makeTest(b, target_khuneo_static);
    }

    if (b.option(bool, "KHUNEO_LIBRARY_SHARED", "Make the khuneo shared library available. default: release false, debug true") orelse is_debug) {
        const target_khuneo_shared = b.addSharedLibrary(.{
            .name = "khuneo-shared",
            .root_source_file = b.path("libkhuneo/src/khuneo.zig"),
            .target = target,
            .optimize = optimize,
            .link_libc = true,
        });
        target_khuneo_shared.root_module.addImport("khuneo", module_khuneo);
        b.installArtifact(target_khuneo_shared);
        makeTest(b, target_khuneo_shared);
    }

    // -------------------------------
    //  Platform for Khuneo's Runtime
    // -------------------------------
    if (b.option(bool, "PKR_BUILD", "Build the platform for khuneo's runtime. default: release false, debug true") orelse is_debug) {
        const PKR_KINDS = enum { CLI, STATIC, SHARED };
        const pkr_kind = b.option(PKR_KINDS, "PKR_KIND", ". default: CLI") orelse PKR_KINDS.CLI;
        const target_pkr = b.addExecutable(.{
            .name = "pkr",
            .root_source_file = b.path("pkr/src/main.zig"),
            .target = target,
            .optimize = optimize,
        });
        target_pkr.root_module.addImport("khuneo", module_khuneo);
        b.installArtifact(target_pkr);

        const artifact_run_khuneo_cli = b.addRunArtifact(target_pkr);
        artifact_run_khuneo_cli.step.dependOn(b.getInstallStep());
        if (b.args) |args| {
            artifact_run_khuneo_cli.addArgs(args);
        }
        const step_run_khuneo_cli = b.step("run:pkr", "Run PKR");
        step_run_khuneo_cli.dependOn(&artifact_run_khuneo_cli.step);

        makeTest(b, target_pkr);
    }
}

pub fn makeTest(b: *std.Build, compile: *std.Build.Step.Compile) void {
    const step = if (b.top_level_steps.get("test")) |step| &step.step else b.step("test", "Run khuneo's full test");
    const unit_test = b.addTest(.{
        .root_source_file = compile.root_module.root_source_file.?,
        .target = compile.root_module.resolved_target.?,
        .optimize = compile.root_module.optimize.?,
    });
    const test_artifact = b.addRunArtifact(unit_test);
    step.dependOn(&test_artifact.step);
}
