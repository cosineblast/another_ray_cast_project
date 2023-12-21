
const c = @import("c.zig");
const std = @import("std");
const assert = std.debug.assert;

const SCREEN_WIDTH = 640;
const SCREEN_HEIGHT = 480;

const Player = @import("player.zig").Player;

const casting = @import("cast.zig");
const SideCastResult = casting.SideCastResult;
const CastResult = casting.CastResult;
const BoundaryCallback = casting.BoundaryCallback;

const Axis = casting.Axis;
const Horizontal = Axis.Horizontal;
const Vertical = Axis.Vertical;

const geometry = @import("vec.zig");
const FPoint = geometry.FPoint;

const Map = @import("map.zig");

pub fn render(renderer: *c.SDL_Renderer, player: *Player, map: *Map) void {
    renderMap(renderer, map);

    renderGrid(renderer);

    renderPlayer2d(renderer, player);

    _ = c.SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);

    var results: [2]SideCastResult = undefined;
    const horizontal_result = &results[Horizontal.index()];
    const vertical_result = &results[Vertical.index()];

    renderBoundaries(renderer, map, player, Horizontal, horizontal_result);

    _= c.SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0x0, 0xff);
    renderDot(renderer, horizontal_result.result_point);

    _ = c.SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0x00, 0xff);
    renderBoundaries(renderer, map, player, Vertical, vertical_result);

    _= c.SDL_SetRenderDrawColor(renderer, 0x0, 0xff, 0xff, 0xff);
    renderDot(renderer, vertical_result.result_point);

    useSideCasts(renderer, map, player, results);
}

fn renderMap(renderer: *c.SDL_Renderer, map: *Map)  void {

    var rect = c.SDL_FRect {
        .x = undefined,
        .y = undefined,
        .w = Map.tile_size,
        .h = Map.tile_size };

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

            x += Map.tile_size;
        }
        x = 0;
        y += Map.tile_size;
    }
}

fn renderGrid(renderer: *c.SDL_Renderer) void {
    // TODO: make this color a constant
    _ = c.SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0x00, 0xff);

    var y: i32 = 0;
    while (y < SCREEN_HEIGHT) : (y += Map.tile_size) {

        var x: i32 = 0;
        while (x < SCREEN_WIDTH) : (x += Map.tile_size) {
            _ = c.SDL_RenderDrawLine(renderer, x, y, x + Map.tile_size, y);
            _ = c.SDL_RenderDrawLine(renderer, x, y, x, y + Map.tile_size);
        }
    }
}

fn renderPlayer2d(renderer: *c.SDL_Renderer, player: *Player) void {
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

fn renderPlayerArrow(renderer: *c.SDL_Renderer, player: *Player) void {
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

fn onBoundaryHit(boundary_point: *FPoint, data: *anyopaque) callconv(.C) void {
    const renderer: *c.SDL_Renderer = @ptrCast(data);

    renderDot(renderer, boundary_point.*);
}

fn renderBoundaries(renderer: *c.SDL_Renderer, map: *Map, player: *Player,
                              axis: Axis, result: *SideCastResult) void {

    var callback = BoundaryCallback {
        .@"fn" = onBoundaryHit,
        .data = @ptrCast(renderer)
    };

    casting.performSingleSideCast(map,
                FPoint{.x = player.x, .y = player.y},
                axis,
                player.angle, result, &callback);
}

fn renderDot(renderer: *c.SDL_Renderer, f: FPoint) void {
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



fn drawResultLine(renderer: *c.SDL_Renderer , player: *Player,
                             cast_result: *CastResult) void {
    const colors = [2]c.SDL_Color {
        .{ .r = 0xff, .g = 0x88, .b = 0xff, .a = 0xff},
        .{ .r = 0xff, .g = 0xff, .b = 0x88, .a = 0xff}
    };

    const line_color = colors[cast_result.hit_axis.index()];

    _ = c.SDL_SetRenderDrawColor(renderer, line_color.r, line_color.g, line_color.b,
                           line_color.a);

    _ = c.SDL_RenderDrawLineF(renderer, player.x, player.y,
                        cast_result.hit_point.x, cast_result.hit_point.y);
}

fn useSideCasts(renderer: *c.SDL_Renderer, map: *Map, player: *Player,
                               side_results: [2]SideCastResult) void{

    var cast_result: CastResult = undefined;

    casting.convertSideResultToCastResult(
        side_results,
        FPoint {.x = player.x, .y = player.y},
        &cast_result
    );

    drawResultLine(renderer, player, &cast_result);

    const maybe_texture = map.getTextureFromTileValue(cast_result.tile);

    if (maybe_texture) |texture| {
        renderTileTexture(renderer, texture);

        renderTileTextureLine(renderer, &cast_result, player);
    }
}

fn renderTileTexture(renderer: *c.SDL_Renderer, texture: *c.SDL_Texture) void {
    var source  = c.SDL_Rect {
        .h = Map.tile_size,
        .w = Map.tile_size,
        .x = 0,
        .y = 0,
    };

    var destination = c.SDL_FRect {
        .h = Map.tile_size,
        .w = Map.tile_size,
        .x = 0,
        .y = 0,
    };

    _ = c.SDL_RenderCopyF(renderer, texture, &source, &destination);
}

fn renderTileTextureLine(renderer: *c.SDL_Renderer,
                           cast_result: *CastResult,
                           player: *Player) void {

    const offset = casting.findTextureLineOffset(cast_result, player.angle);

    _ = c.SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);

    _ = c.SDL_RenderDrawLineF(renderer, offset, 0, offset, Map.tile_size);
}
