
const std = @import("std");
const c = @import("c.zig");

pub var time_variation: f32 = 0.0;

var last_time_measure: u32 = 0;
var debug_last_second_measure: u32 = 0;

pub fn updateTime() void {
    const last = last_time_measure;
    const current = c.SDL_GetTicks();

    time_variation = @as(f32, @floatFromInt(current - last)) / 1000.0;

    last_time_measure = current;

    if (current > debug_last_second_measure + 1000) {
        std.debug.print("FPS: {}\n", .{1.0 / time_variation});
        debug_last_second_measure = current;
    }
}

