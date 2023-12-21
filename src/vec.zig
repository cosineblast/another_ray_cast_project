const std = @import("std");
const c = @import("c.zig");


pub const FVec2 = struct {
    x: f32,
    y: f32,

    pub fn norm(vec: FVec2) f32 {
        return std.math.sqrt(vec.x * vec.x + vec.y * vec.y);
    }

    pub fn dot(u: FVec2, v: FVec2) f32 {
        return u.x * v.x + u.y * v.y;
    }
};

pub fn addPoint(p: *c.SDL_FPoint, vec: FVec2) void {
    p.x += vec.x;
    p.y += vec.y;
}

pub fn pointDifference(p: c.SDL_FPoint, q: c.SDL_FPoint) FVec2 {
    return .{ .x = q.x - p.x, .y = q.y - p.y };
}

pub fn pointDistance(first: c.SDL_FPoint , second: c.SDL_FPoint) f32 {
    return pointDifference(first, second).norm();
}
