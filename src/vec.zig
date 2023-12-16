const std = @import("std");
const c = @import("c.zig");

export fn point_add(p: *c.SDL_FPoint, vec: c.FVec2) void {
    p.x += vec.x;
    p.y += vec.y;
}

export fn point_difference(p: c.SDL_FPoint, q: c.SDL_FPoint) c.FVec2 {
    return .{ .x = q.x - p.x, .y = q.y - p.y };
}

export fn vec_dot(u: c.FVec2, v: c.FVec2) f32 {
    return u.x * v.x + u.y * v.y;
}

export fn vec_norm(vec: c.FVec2) f32 {
    return std.math.sqrt(vec.x * vec.x + vec.y * vec.y);
}
