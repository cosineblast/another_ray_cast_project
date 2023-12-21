const std = @import("std");
const c = @import("c.zig");
const assert = std.debug.assert;

const geometry = @import("geometry.zig");
const FPoint = geometry.FPoint;

const Allocator = std.mem.Allocator;

rows: usize,
cols: usize,
tiles: [*]u8,
textures: [*]*c.SDL_Texture,

pub const Map = @This();
pub const tile_size = 64;


pub fn makeSampleMap(alloc: Allocator, renderer: *c.SDL_Renderer) !*Map {
    const map = try alloc.create(Map);
    map.rows = 10;
    map.cols = 10;

    const tiles = try alloc.alloc(u8, map.rows * map.cols);

    const rawTiles: [*]u8 = tiles.ptr;
    map.tiles = rawTiles;

    const sample_map =
        [_]u8{
        2, 1, 2, 1, 2, 1, 2, 1, 2, 1, //
        1, 0, 0, 0, 0, 0, 0, 0, 0, 2, //
        2, 0, 0, 0, 0, 0, 0, 0, 0, 1, //
        1, 0, 0, 0, 0, 0, 0, 0, 0, 2, //
        2, 0, 0, 0, 0, 0, 0, 0, 0, 1, //
        1, 0, 0, 0, 0, 0, 0, 0, 0, 2, //
        2, 0, 0, 0, 0, 0, 0, 1, 0, 1, //
        1, 0, 0, 0, 0, 0, 1, 2, 0, 2, //
        2, 0, 0, 0, 0, 0, 0, 0, 0, 1, //
        1, 2, 1, 2, 1, 2, 1, 2, 1, 2, //
    };

    assert(sample_map.len == map.rows * map.cols);
    @memcpy(map.tiles[0..sample_map.len], sample_map[0..]);

    try initializeTextures(alloc, map, renderer);

    return map;
}

const texture_paths =
    [_][*:0]const u8{ "./assets/beep.png", "./assets/boop.png" };

fn initializeTextures(alloc: Allocator, map: *Map, renderer: *c.SDL_Renderer) !void {
    const textures = try alloc.alloc(*c.SDL_Texture, texture_paths.len);
    map.textures = textures.ptr;

    for (0..texture_paths.len) |i| {
        map.textures[i] = try loadTexture(texture_paths[i], renderer);
    }
}

fn loadTexture(path: [*:0]const u8, renderer: *c.SDL_Renderer) !*c.SDL_Texture {
    const surface = c.IMG_Load(path) orelse {
        std.log.err("failed to load surface {s}: {s}\n", .{ path, c.SDL_GetError() });
        return error.SDLTextureError;
    };

    const texture = c.SDL_CreateTextureFromSurface(renderer, surface) orelse {
        std.log.err("failed to load texture: {s}\n", .{c.SDL_GetError()});
        return error.SDLTextureError;
    };

    return texture;
}

pub fn deinit( map: *Map, alloc: Allocator,) void {
    alloc.free(map.tiles[0..(map.rows * map.cols)]);
    alloc.destroy(map);
}

pub fn getTextureFromTileValue(map: *Map, tile_value_: ?u8) ?*c.SDL_Texture {
    const tile_value = tile_value_ orelse return null;

    if (tile_value == 0) {
        return null;
    }

    assert(tile_value <= texture_paths.len);

    const texture_index: usize = @intCast(tile_value - 1);

    return map.textures[texture_index];
}


pub fn findWallAtPoint(map: *Map, point: FPoint) ?u8 {
    const row = @divFloor(@as(isize, @intFromFloat(point.y)), @as(isize, tile_size));
    const col = @divFloor(@as(isize, @intFromFloat(point.x)), @as(isize, tile_size));

    if (row >= 0 and col >= 0 and @as(usize, @intCast(row)) < map.rows and
        @as(usize, @intCast(col)) < map.cols)
    {
        const urow: usize = @intCast(row);
        const ucol: usize = @intCast(col);

        return map.tiles[urow * map.cols + ucol];
    } else {
        return null;
    }
}
