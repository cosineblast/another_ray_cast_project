const c = @import("c.zig");
const std = @import("std");

const view = @import("view.zig");
const timing = @import("timing.zig");
const preview = @import("preview.zig");

pub const Player = @This();

x: f32,
y: f32,
angle: f32,

pub fn move(player: *Player) !void {
    const keyboard_state = c.SDL_GetKeyboardState(null) orelse return error.SDLKeyboardFailed;

    const movement_speed: f32 = 600.0 * timing.time_variation;
    const angular_speed: f32 = 1 * timing.time_variation;

    player.angle += angular_speed * @as(f32, @floatFromInt(keyboard_state[c.SDL_SCANCODE_A]));
    player.angle -= angular_speed * @as(f32, @floatFromInt(keyboard_state[c.SDL_SCANCODE_D]));

    const angle_sine: f32 = std.math.sin(player.angle);
    const angle_cosine: f32 = std.math.cos(player.angle);

    player.x += angle_cosine * movement_speed * @as(f32, @floatFromInt(keyboard_state[c.SDL_SCANCODE_W]));
    player.y -= angle_sine * movement_speed * @as(f32, @floatFromInt(keyboard_state[c.SDL_SCANCODE_W]));

    player.x -= angle_cosine * movement_speed * @as(f32, @floatFromInt(keyboard_state[c.SDL_SCANCODE_S]));
    player.y += angle_sine * movement_speed * @as(f32, @floatFromInt(keyboard_state[c.SDL_SCANCODE_S]));
}

pub fn render(player: *Player, renderer: *c.SDL_Renderer, map: *c.Map) void {
    // here we pick if we want renderPlayerView or renderPlayerPreview

    view.renderPlayerView(renderer, player, map);
    preview.render(renderer, player, map);
}

pub fn initWithSampleData() Player {
    return .{ .x = 150, .y = 250, .angle = std.math.pi / 2.0 };
}
