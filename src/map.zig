const std = @import("std");
const c = @import("c.zig");
const assert = std.debug.assert;

const Allocator = std.mem.Allocator;

pub fn make_sample_map(alloc: Allocator, renderer: *c.SDL_Renderer) !*c.Map {
    const map = try alloc.create(c.Map);
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

    c.initialize_textures(map, renderer);

    return map;
}


pub fn deinit(alloc: Allocator, map: *c.Map) void {
    alloc.free(map.tiles[0..(map.rows * map.cols)]);
    alloc.destroy(map);
}

export fn map_free(map: *c.Map) void {
    deinit(std.heap.raw_c_allocator, map);
}


export fn map_new_sample(renderer: *c.SDL_Renderer) ?*c.Map {
    return make_sample_map(std.heap.raw_c_allocator, renderer) catch std.process.exit(1);
}
