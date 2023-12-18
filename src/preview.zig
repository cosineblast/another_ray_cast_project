
const c = @import("c.zig");
const std = @import("std");
const assert = std.debug.assert;

const SCREEN_WIDTH = 640;
const SCREEN_HEIGHT = 480;

pub fn render(renderer: *c.SDL_Renderer, player: *c.Player, map: *c.Map) void {
    renderMap(renderer, map);

    renderGrid(renderer);

    renderPlayer2d(renderer, player);

    _ = c.SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);

    var results: [2]c.SideCastResult = undefined;
    const horizontal_result = &results[c.HORIZONTAL];
    const vertical_result = &results[c.VERTICAL];

    renderBoundaries(renderer, map, player, c.HORIZONTAL, horizontal_result);

    _= c.SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0x0, 0xff);
    renderDot(renderer, horizontal_result.result_point);

    _ = c.SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0x00, 0xff);
    renderBoundaries(renderer, map, player, c.VERTICAL, vertical_result);

    _= c.SDL_SetRenderDrawColor(renderer, 0x0, 0xff, 0xff, 0xff);
    renderDot(renderer, vertical_result.result_point);

    useSideCasts(renderer, map, player, &results);
}

fn renderMap(renderer: *c.SDL_Renderer, map: *c.Map)  void {

    var rect = c.SDL_FRect {
        .x = undefined,
        .y = undefined,
        .w = c.TILE_SIZE,
        .h = c.TILE_SIZE };

    var y: i32 = 0;
    for (0..SCREEN_HEIGHT) |row| {

        var x: i32 = 0;
        for (0..SCREEN_HEIGHT) |col| {
            const tile = map.tiles[row * map.cols + col];

            if (tile != 0) {
                rect.x = @floatFromInt(x);
                rect.y = @floatFromInt(y);

                // TODO: call function to get color from tile
                _ = c.SDL_SetRenderDrawColor(renderer, 0x28, 0x2C, 0x34, 0xff);
                _ = c.SDL_RenderFillRectF(renderer, &rect);
            }

            x += c.TILE_SIZE;
        }
        x = 0;
        y += c.TILE_SIZE;
    }
}

fn renderGrid(renderer: *c.SDL_Renderer) void {
    // TODO: make this color a constant
    _ = c.SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0x00, 0xff);

    var y: i32 = 0;
    while (y < SCREEN_HEIGHT) : (y += c.TILE_SIZE) {

        var x: i32 = 0;
        while (x < SCREEN_WIDTH) : (x += c.TILE_SIZE) {
            _ = c.SDL_RenderDrawLine(renderer, x, y, x + c.TILE_SIZE, y);
            _ = c.SDL_RenderDrawLine(renderer, x, y, x, y + c.TILE_SIZE);
        }
    }
}

fn renderPlayer2d(renderer: *c.SDL_Renderer, player: *c.Player) void {
    renderPlayerArrow(renderer, player);

    _ = c.SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0x00, 0xff);

    const width = 10.0;
    const height = 10.0;
    const rectangle = c.SDL_FRect{
        .x = player.x - width / 2.0,
        .y = player.y - height / 2.0,
        .w = width,
        .h = height
    };

    _ = c.SDL_RenderFillRectF(renderer, &rectangle);
}

fn renderPlayerArrow(renderer: *c.SDL_Renderer, player: *c.Player) void {
    const arrow_size: f32 = 50;

    const sine: f32 = std.math.sin(player.angle);
    const cosine: f32 = std.math.cos(player.angle);

    const arrow_head_x: f32 = player.x + arrow_size * cosine;
    const arrow_head_y: f32 = player.y - arrow_size * sine;

    _ = c.SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xff, 0xff);
    _ = c.SDL_RenderDrawLine(
        renderer,
        @intFromFloat(player.x),
        @intFromFloat(player.y),
        @intFromFloat(arrow_head_x),
        @intFromFloat(arrow_head_y)
    );
}

fn onBoundaryHit(boundary_point: [*c]c.SDL_FPoint, data: ?*anyopaque) callconv(.C) void {
    const renderer: *c.SDL_Renderer = @ptrCast(data);

    renderDot(renderer, boundary_point.*);
}

fn renderBoundaries(renderer: *c.SDL_Renderer, map: *c.Map, player: *c.Player,
                              is_vertical: i32, result: *c.SideCastResult) void {

    var callback = c.BoundaryCallback {
        .@"fn" = onBoundaryHit,
        .data = @ptrCast(renderer)
    };

    c.cast_side(map,
                c.SDL_FPoint{.x = player.x, .y = player.y},
                is_vertical,
                player.angle, result, &callback);
}

fn renderDot(renderer: *c.SDL_Renderer, f: c.SDL_FPoint) void {
    const width = 5.0;
    const height = 5.0;

    const rect = c.SDL_FRect {
        .w = width,
        .h = height,
        .x = f.x - width / 2.0,
        .y = f.y - height / 2.0
    };

    _ = c.SDL_RenderFillRectF(renderer, &rect);
}



fn drawResultLine(renderer: *c.SDL_Renderer , player: *c.Player,
                             cast_result: *c.CastResult) void {
    const colors = [2]c.SDL_Color {
        .{ .r = 0xff, .g = 0x88, .b = 0xff, .a = 0xff},
        .{ .r = 0xff, .g = 0xff, .b = 0x88, .a = 0xff}
    };

    assert(cast_result.is_vertical == 0 or cast_result.is_vertical == 1);

    const line_color = colors[@intCast(cast_result.is_vertical)];

    _ = c.SDL_SetRenderDrawColor(renderer, line_color.r, line_color.g, line_color.b,
                           line_color.a);

    _ = c.SDL_RenderDrawLineF(renderer, player.x, player.y,
                        cast_result.hit_point.x, cast_result.hit_point.y);
}

fn useSideCasts(renderer: *c.SDL_Renderer, map: *c.Map, player: *c.Player,
                               side_results: [*]c.SideCastResult) void{

    var cast_result: c.CastResult = undefined;

    c.cast_result_from_sides(
        side_results,
        c.SDL_FPoint {.x = player.x, .y = player.y},
        &cast_result
    );

    drawResultLine(renderer, player, &cast_result);

    const maybe_texture = c.map_texture_from_tile_value(map, cast_result.tile);

    if (maybe_texture) |texture| {
        renderTileTexture(renderer, texture);

        renderTileTextureLine(renderer, &cast_result, player);
    }
}

fn renderTileTexture(renderer: *c.SDL_Renderer, texture: *c.SDL_Texture) void {
    var source  = c.SDL_Rect {
        .h = c.TILE_SIZE,
        .w = c.TILE_SIZE,
        .x = 0,
        .y = 0,
    };

    var destination = c.SDL_FRect {
        .h = c.TILE_SIZE,
        .w = c.TILE_SIZE,
        .x = 0,
        .y = 0,
    };

    _ = c.SDL_RenderCopyF(renderer, texture, &source, &destination);
}

fn renderTileTextureLine(renderer: *c.SDL_Renderer,
                           cast_result: *c.CastResult,
                           player: *c.Player) void {

    const offset = c.cast_find_texture_line_offset(cast_result, player.angle);

    _ = c.SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);

    _ = c.SDL_RenderDrawLineF(renderer, offset, 0, offset, c.TILE_SIZE);
}
