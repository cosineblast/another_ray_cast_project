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

pub const FPoint = struct {
    x: f32,
    y: f32,
};

pub fn addPoint(p: *FPoint, vec: FVec2) void {
    p.x += vec.x;
    p.y += vec.y;
}

pub fn pointDifference(p: FPoint, q: FPoint) FVec2 {
    return .{ .x = q.x - p.x, .y = q.y - p.y };
}

pub fn pointDistance(first: FPoint , second: FPoint) f32 {
    return pointDifference(first, second).norm();
}
