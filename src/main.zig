

const std = @import("std");
const c = @import("c.zig");

const timing  = @import("timing.zig");

fn run_main_loop(map: *c.Map, player: *c.Player, renderer: ?*c.SDL_Renderer) void {
    var event: c.SDL_Event = undefined;

    var running: bool = true;

    while (running) {
        while (c.SDL_PollEvent(&event) != 0) {
            if (event.type == c.SDL_QUIT) {
                running = false;
            }
        }

        timing.update_time();

        c.player_move(renderer, player);

        _ = c.SDL_SetRenderDrawColor(renderer, 0x0d, 0x11, 0x17, 0xff);

        _ = c.SDL_RenderClear(renderer);

        c.player_render(renderer, player, map);

        _ = c.SDL_RenderPresent(renderer);
    }
}

pub fn main() !void {
    _ = c.SDL_Init(c.SDL_INIT_VIDEO);
    defer c.SDL_Quit();

    _ = c.IMG_Init(c.IMG_INIT_PNG);
    defer c.IMG_Quit();

    const window = c.SDL_CreateWindow("hi there",
                                      c.SDL_WINDOWPOS_CENTERED,
                                      c.SDL_WINDOWPOS_CENTERED,
                                      640, 480,
                                      c.SDL_WINDOW_HIDDEN)
        orelse return error.SDLWindowFailed;

    defer c.SDL_DestroyWindow(window);

    const renderer = c.SDL_CreateRenderer(
        window, -1, c.SDL_RENDERER_ACCELERATED | c.SDL_RENDERER_PRESENTVSYNC)
        orelse return error.SDLRendererFailed;

    defer c.SDL_DestroyRenderer(renderer);

    c.SDL_ShowWindow(window);

    const map = c.map_new_sample(renderer);
    defer c.map_free(map);

    var player: c.Player = undefined;

    c.player_init_sample(&player);

    run_main_loop(map, &player, renderer);
}

usingnamespace @import("timing.zig");