const std = @import("std");
const assert = std.debug.assert;
const c = @import("c.zig");

const USE_TEXTURES = false;

const FOV = 1.04719;
const SCREEN_WIDTH = 640;
const SCREEN_HEIGHT = 480;

const casting = @import("cast.zig");

const ColumnRenderArguments = struct {
    map: *c.Map,
    cast_result: *c.CastResult,
    column_rectangle: *const c.SDL_FRect,
    renderer: *c.SDL_Renderer,
    current_angle: f32
};

const Player = @import("player.zig").Player;

fn renderRaycastColumn(args: *const ColumnRenderArguments) void {
    if (comptime blk: { break :blk USE_TEXTURES; }) {
        renderTextureRaycastColumn(args);
    }
    else {
        renderColoredRaycastColumn(args);
    }
}

fn renderTextureRaycastColumn(args: *const ColumnRenderArguments) void {
    const maybe_texture = c.map_texture_from_tile_value(args.map, args.cast_result.tile);

    if (maybe_texture) |texture| {
        const source =
            c.SDL_Rect{
            .x = @intFromFloat(casting.findTextureLineOffset(args.cast_result, args.current_angle)),
            .y = 0,
            .w = 1,
            .h = c.TILE_SIZE,
        };

        _ = c.SDL_RenderCopyF(args.renderer, texture, &source, args.column_rectangle);
    } else {
        renderColoredRaycastColumn(args);
    }
}

export fn renderColoredRaycastColumn(args: *const ColumnRenderArguments) void {
    var color: c.SDL_Color = undefined;
    getTileColor(args.cast_result.tile, &color);

    _ = c.SDL_SetRenderDrawColor(args.renderer, color.r, color.g, color.b, color.a);

    _ = c.SDL_RenderFillRectF(args.renderer, args.column_rectangle);
}

pub fn renderPlayerView(renderer: *c.SDL_Renderer, player: *Player, map: *c.Map) void {
    var current_angle = player.angle + FOV / 2.0;

    const angle_increment = @as(f32, FOV) / SCREEN_WIDTH;

    for (1..SCREEN_WIDTH + 1) |current_column| {
        var result: c.CastResult = undefined;

        casting.performRayCast(map, .{ .x = player.x, .y = player.y }, current_angle, &result);

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

        renderRaycastColumn(&ColumnRenderArguments {
            .map = map,
            .cast_result = &result,
            .renderer = renderer,
            .current_angle = current_angle,
            .column_rectangle = &rectangle
        });

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
