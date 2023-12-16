const std = @import("std");
const assert = std.debug.assert;
const c = @import("c.zig");

const FOV = 1.04719;

const SCREEN_WIDTH = 640;
const SCREEN_HEIGHT = 480;

// the render_texture_raycast_column and render_colored_raycast_column
// are both functions that take a rectangle (and quite few other things)
// and render the player view on such rectangle on screen based on
// factors such as the hitting angle and hitting tile
//
// we could implement a better system for choosing the rendering function
// but uh this suffices

// todo: make these arguments into a struct

fn renderTextureRaycastColumn(map: *c.Map, result: *c.CastResult, column_rectangle: *const c.SDL_FRect, renderer: *c.SDL_Renderer, current_angle: f32) void {
    const maybe_texture = c.map_texture_from_tile_value(map, result.tile);

    if (maybe_texture) |texture| {
        const source =
            c.SDL_Rect{
            .x = @intFromFloat(c.cast_find_texture_line_offset(result, current_angle)),
            .y = 0,
            .w = 1,
            .h = c.TILE_SIZE,
        };

        _ = c.SDL_RenderCopyF(renderer, texture, &source, column_rectangle);
    } else {
        c.render_colored_raycast_column(map, result, column_rectangle, renderer, current_angle);
    }
}

export fn renderColoredRaycastColumn(map: *c.Map, result: *c.CastResult, column_rectangle: *const c.SDL_FRect, renderer: *c.SDL_Renderer, current_angle: f32) void {
    _ = current_angle;
    _ = map;

    var color: c.SDL_Color = undefined;
    getTileColor(result.tile, &color);

    _ = c.SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    _ = c.SDL_RenderFillRectF(renderer, column_rectangle);
}

pub fn renderPlayerView(renderer: *c.SDL_Renderer, player: *c.Player, map: *c.Map) void {
    var current_angle = player.angle + FOV / 2.0;

    const angle_increment = @as(f32, FOV) / SCREEN_WIDTH;

    for (1..SCREEN_WIDTH + 1) |current_column| {
        var result: c.CastResult = undefined;

        c.cast_full(map, .{ .x = player.x, .y = player.y }, current_angle, &result);

        const distance = result.distance * std.math.cos(player.angle - current_angle);

        const height = SCREEN_HEIGHT * 50.0 / distance;

        const middle_y = SCREEN_HEIGHT / 2.0 - height / 2.0;

        const rectangle =
            c.SDL_FRect{
            .x = @floatFromInt(current_column),
            .y = middle_y,
            .w = 1,
            .h = height,
        };

        renderColoredRaycastColumn(map, &result, &rectangle, renderer, current_angle);

        current_angle -= angle_increment;
    }
}

const color_mapping =
    [_]c.SDL_Color{ undefined, .{ .r = 0x28, .g = 0x2c, .b = 0x34, .a = 0xff }, .{ .r = 0xf7, .g = 0x81, .b = 0xA9, .a = 0xff } };

export fn getTileColor(tile: i8, color: *c.SDL_Color) void {
    assert(tile != 0);
    assert(tile >= -1);
    assert(tile <= 2);

    if (tile == -1) {
        color.* = c.SDL_Color{ .r = 0xff, .g = 0xff, .b = 0xff, .a = 0xff };
    } else {
        color.* = color_mapping[@intCast(tile)];
    }
}