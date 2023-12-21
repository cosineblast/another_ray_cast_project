const c = @import("c.zig");
const std = @import("std");

const SCREEN_WIDTH = 640;
const SCREEN_HEIGHT = 480;

const TILE_SIZE: comptime_int = @intCast(c.TILE_SIZE);

const Map = @import("map.zig");

pub const Axis = enum (u8) {
    Horizontal = 0,
    Vertical = 1,

    pub fn index(self: Axis) u8 {
        return @intFromEnum(self);
    }
};

pub const CastResult = struct {
    hit_point: c.SDL_FPoint,
    tile: i8,
    hit_axis: Axis,
    distance: f32,
};


pub const SideCastResult = struct {
    result_point: c.SDL_FPoint,
    tile: i8,
};

pub const BoundaryCallback = struct {
    @"fn": *const fn(point: *c.SDL_FPoint, data: *anyopaque) callconv(.C) void,
    data: *anyopaque,
};




// Performs a ray cast computation with the given parameters.
//
// This function may answer the following question:
// Tracing a straight line from the given point P and angle Alpha, what
// is the distance to the first wall it its on the map?
//
// The resulting travel distance, alongside other information is saved
// on the fields of `output`. See CastResult for more information.
pub fn performRayCast(map: *Map, source_point: c.SDL_FPoint , angle: f32,
               output: *CastResult) void {
    var side_cast_results: [2]SideCastResult = undefined;

    performSingleSideCast(map, source_point, Axis.Horizontal, angle,
              &side_cast_results[Axis.Horizontal.index()], null);

    performSingleSideCast(map, source_point, Axis.Vertical, angle, &side_cast_results[Axis.Vertical.index()],
              null);

    convertSideResultToCastResult(side_cast_results, source_point, output);
}



// Performs a ray cast computation with the given cast parameters, but only
// considers walls that match the given orientation (vertical or horizontal).
//
// This function may answer the following question:
// Tracing a straight line from the given point P and angle Alpha, what
// is the distance to to the first wall it hits on the map, that has
// the given orientation?
//
// The resulting travel distance, alongside other information is saved
// on the fields of `output`. See CastResult for more information.
//
// See also: performRayCast
//
pub fn performSingleSideCast(map: *Map, source_point: c.SDL_FPoint, axis: Axis, angle: f32,
                 result: *SideCastResult, callback: ?*BoundaryCallback) void {
    var start_point: c.SDL_FPoint = undefined;

    var advancement: c.FVec2 = undefined;

    var displacement: c.FVec2 = undefined;

    if (axis == Axis.Vertical) {
        findVerticalBoundary(source_point, angle, &start_point, &advancement);

        findVerticalDisplacement(angle, &displacement);
    } else {
        findHorizontalBoundary(source_point, angle, &start_point, &advancement);

        findHorizontalDisplacement(angle, &displacement);
    }

    runSideCast(map, start_point, advancement, displacement, result, callback);
}

pub fn convertSideResultToCastResult(results: [2]SideCastResult, source_point: c.SDL_FPoint, output: *CastResult) void {
    var distances: [2]f32 = undefined;

    for (0..2) |i|  {
        distances[i] = pointDistance(source_point, results[i].result_point);
    }

    const shortest_axis: Axis =
        if (distances[Axis.Horizontal.index()] < distances[Axis.Vertical.index()])
        Axis.Horizontal else Axis.Vertical;

    const shortest_index  = shortest_axis.index();

    output.distance = distances[shortest_index];
    output.hit_point = results[shortest_index].result_point;
    output.hit_axis = shortest_axis;
    output.tile = results[shortest_index].tile;
}

pub fn findTextureLineOffset(result: *CastResult, cast_angle: f32) f32 {
    const sine = std.math.sin(cast_angle);
    const cosine = std.math.cos(cast_angle);

    if (result.hit_axis == Axis.Vertical) {
        const mod = @rem(@as(i32, @intFromFloat(result.hit_point.y)), TILE_SIZE);
        if (cosine < 0.0) {
            return @floatFromInt(TILE_SIZE - mod);
        } else {
            return @floatFromInt(mod);
        }
    } else {
        const mod = @rem(@as(i32, @intFromFloat(result.hit_point.x)), TILE_SIZE);
        if (sine < 0.0) {
            return @floatFromInt(TILE_SIZE - mod);
        } else {
            return @floatFromInt(mod);
        }
    }
}

fn findHorizontalDisplacement(angle: f32, displacement: *c.FVec2) void {
    displacement.x = 0.0;
    displacement.y = -std.math.copysign(@as(f32, @floatFromInt(TILE_SIZE)) / 2.0, std.math.sin(angle));
}

fn findVerticalDisplacement(angle: f32, displacement: *c.FVec2) void {
    displacement.x = std.math.copysign(@as(f32, @floatFromInt(TILE_SIZE)) / 2.0, std.math.cos(angle));
    displacement.y = 0.0;
}

fn pointDistance(first: c.SDL_FPoint , second: c.SDL_FPoint) f32 {
    return c.vec_norm(c.point_difference(first, second));
}

fn findHorizontalBoundary(point: c.SDL_FPoint, angle: f32, result: *c.SDL_FPoint, boundary_distance: *c.FVec2) void {

    const cotangent = 1.0 / std.math.tan(angle);
    const sine = std.math.sin(angle);

    boundary_distance.y = std.math.copysign(@as(f32, TILE_SIZE), sine);
    boundary_distance.x = boundary_distance.y * cotangent;

    const height: f32 = @floatFromInt(@rem(@as(i32, @intFromFloat(point.y)), 64));

    var y = point.y - height;

    const base = height * cotangent;

    var x = point.x + base;

    if (sine < 0.0) {
        y = y - boundary_distance.y;
        x = x + boundary_distance.x;
    }

    result.x = x;
    result.y = y;

    boundary_distance.y *= -1.0;
}

fn findVerticalBoundary(point: c.SDL_FPoint, angle: f32, result: *c.SDL_FPoint, boundary_distance: *c.FVec2) void {

    const tangent = std.math.tan(angle);
    const cosine = std.math.cos(angle);

    boundary_distance.x = std.math.copysign(@as(f32, TILE_SIZE), cosine);
    boundary_distance.y = boundary_distance.x * tangent;

    const width: f32 = @floatFromInt(@rem(@as(i32, @intFromFloat(point.x)), 64));

    var x = point.x - width;

    const top = width * tangent;

    var y = point.y + top;

    if (cosine > 0.0) {
        y = y - boundary_distance.y;
        x = x + boundary_distance.x;
    }

    result.x = x;
    result.y = y;

    boundary_distance.y *= -1.0;
}

fn runSideCast(map: *Map, start_point: c.SDL_FPoint,
                      advancement: c.FVec2, lookup_displacement: c.FVec2,
                      result: *SideCastResult, callback: ?*BoundaryCallback) void {
    var current_point: c.SDL_FPoint = start_point;

    var point_inside_block: c.SDL_FPoint = undefined;

    while (true) {
        point_inside_block = current_point;

        c.point_add(&point_inside_block, lookup_displacement);

        const tile = Map.findIntersectingWall(map, point_inside_block);

        if (tile != 0) {
            result.tile = tile;
            break;
        }

        if (current_point.x < 0 or current_point.x > SCREEN_WIDTH or
            current_point.y < 0 or current_point.y > SCREEN_HEIGHT) {
            result.tile = -1;
            break;
        }

        if (callback) |callback2| {
            const function = callback2.@"fn";
            function(&current_point, callback2.data);
        }

        c.point_add(&current_point, advancement);
    }

    result.result_point = current_point;
}

