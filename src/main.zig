const std = @import("std");
const c = @import("c.zig");

const timing = @import("timing.zig");
const field = @import("map.zig");
const Player = @import("player.zig");

fn runMainLoop(map: *c.Map, player: *Player, renderer: *c.SDL_Renderer) !void {
    var event: c.SDL_Event = undefined;

    var running: bool = true;

    while (running) {
        while (c.SDL_PollEvent(&event) != 0) {
            if (event.type == c.SDL_QUIT) {
                running = false;
            }
        }

        timing.updateTime();

        try player.move();

        _ = c.SDL_SetRenderDrawColor(renderer, 0x0d, 0x11, 0x17, 0xff);

        _ = c.SDL_RenderClear(renderer);

        player.render(renderer, map);

        _ = c.SDL_RenderPresent(renderer);
    }
}

pub fn main() !void {
    _ = c.SDL_Init(c.SDL_INIT_VIDEO);
    defer c.SDL_Quit();

    _ = c.IMG_Init(c.IMG_INIT_PNG);
    defer c.IMG_Quit();

    const window = c.SDL_CreateWindow("hi there", c.SDL_WINDOWPOS_CENTERED, c.SDL_WINDOWPOS_CENTERED, 640, 480, c.SDL_WINDOW_HIDDEN) orelse return error.SDLWindowFailed;

    defer c.SDL_DestroyWindow(window);

    const renderer = c.SDL_CreateRenderer(window, -1, c.SDL_RENDERER_ACCELERATED | c.SDL_RENDERER_PRESENTVSYNC) orelse return error.SDLRendererFailed;

    defer c.SDL_DestroyRenderer(renderer);

    c.SDL_ShowWindow(window);

    const allocator = std.heap.c_allocator;

    const map = try field.makeSampleMap(allocator, renderer);
    defer field.deinit(allocator, map);

    var player = Player.initWithSampleData();

    try runMainLoop(map, &player, renderer);
}

usingnamespace @import("map.zig");
usingnamespace @import("timing.zig");
usingnamespace @import("vec.zig");
usingnamespace @import("player.zig");
usingnamespace @import("view.zig");
usingnamespace @import("preview.zig");
usingnamespace @import("cast.zig");
