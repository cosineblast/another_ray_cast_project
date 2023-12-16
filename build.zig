const std = @import("std");

pub fn build(b: *std.Build) void {

    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(std.build.ExecutableOptions {
        .root_source_file = .{ .path = "src/main.zig" },
        .name = "main",
        .target = target,
        .optimize = optimize,
    });

    exe.addCSourceFiles(&.{
        "src/player.c",
        "src/view.c",
        "src/vec.c",
        "src/preview.c",
        "src/cast.c",
    }, &.{"-Wall"});

    exe.linkLibC();
    exe.addIncludePath(.{ .path = "src" });
    exe.linkSystemLibrary("SDL2");
    exe.linkSystemLibrary("SDL2_image");

    b.installArtifact(exe);

    b.installDirectory(std.build.InstallDirectoryOptions {
        .source_dir = std.build.LazyPath { .path = "assets" },
        .install_dir = std.build.InstallDir.bin,
        .install_subdir = "assets"
    });

    addRunCommand(b, exe);
}


fn addRunCommand(b: *std.Build, exe: *std.build.Step.Compile) void {
    const run_cmd = b.addRunArtifact(exe);

    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
}
