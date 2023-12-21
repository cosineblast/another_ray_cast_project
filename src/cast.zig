const c = @import("c.zig");
const std = @import("std");


const SCREEN_WIDTH = 640;
const SCREEN_HEIGHT = 480;


const Map = @import("map.zig");
const geometry = @import("geometry.zig");

const FPoint = geometry.FPoint;
const FVec2 = geometry.FVec2;

pub const Axis = enum (u8) {
    Horizontal = 0,
    Vertical = 1,

    pub fn index(self: Axis) u8 {
        return @intFromEnum(self);
    }
};

pub const CastResult = struct {
    hit_point: FPoint,
    tile: ?u8,
    hit_axis: Axis,
    distance: f32,
};


pub const SideCastResult = struct {
    result_point: FPoint,
    tile: ?u8,
};

pub const BoundaryCallback = struct {
    @"fn": *const fn(point: *FPoint, data: *anyopaque) callconv(.C) void,
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
pub fn performRayCast(map: *Map, source_point: FPoint , angle: f32,
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
pub fn performSingleSideCast(map: *Map, source_point: FPoint, axis: Axis, angle: f32,
                 result: *SideCastResult, callback: ?*BoundaryCallback) void {
    var start_point: FPoint = undefined;

    var advancement: FVec2 = undefined;

    var displacement: FVec2 = undefined;

    if (axis == Axis.Vertical) {
        findVerticalBoundary(source_point, angle, &start_point, &advancement);

        findVerticalDisplacement(angle, &displacement);
    } else {
        findHorizontalBoundary(source_point, angle, &start_point, &advancement);

        findHorizontalDisplacement(angle, &displacement);
    }

    runSideCast(map, start_point, advancement, displacement, result, callback);
}

pub fn convertSideResultToCastResult(results: [2]SideCastResult, source_point: FPoint, output: *CastResult) void {
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
        const mod = @rem(@as(i32, @intFromFloat(result.hit_point.y)), Map.tile_size);
        if (cosine < 0.0) {
            return @floatFromInt(Map.tile_size - mod);
        } else {
            return @floatFromInt(mod);
        }
    } else {
        const mod = @rem(@as(i32, @intFromFloat(result.hit_point.x)), Map.tile_size);
        if (sine < 0.0) {
            return @floatFromInt(Map.tile_size - mod);
        } else {
            return @floatFromInt(mod);
        }
    }
}

fn findHorizontalDisplacement(angle: f32, displacement: *FVec2) void {
    displacement.x = 0.0;
    displacement.y = -std.math.copysign(@as(f32, @floatFromInt(Map.tile_size)) / 2.0, std.math.sin(angle));
}

fn findVerticalDisplacement(angle: f32, displacement: *FVec2) void {
    displacement.x = std.math.copysign(@as(f32, @floatFromInt(Map.tile_size)) / 2.0, std.math.cos(angle));
    displacement.y = 0.0;
}

fn pointDistance(first: FPoint , second: FPoint) f32 {
    return first.difference(second).norm();
}

fn findHorizontalBoundary(point: FPoint, angle: f32, result: *FPoint, boundary_distance: *FVec2) void {

    const cotangent = 1.0 / std.math.tan(angle);
    const sine = std.math.sin(angle);

    boundary_distance.y = std.math.copysign(@as(f32, Map.tile_size), sine);
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

fn findVerticalBoundary(point: FPoint, angle: f32, result: *FPoint, boundary_distance: *FVec2) void {

    const tangent = std.math.tan(angle);
    const cosine = std.math.cos(angle);

    boundary_distance.x = std.math.copysign(@as(f32, Map.tile_size), cosine);
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

fn runSideCast(map: *Map, start_point: FPoint,
                      advancement: FVec2, lookup_displacement: FVec2,
                      result: *SideCastResult, callback: ?*BoundaryCallback) void {
    var current_point: FPoint = start_point;

    var point_inside_block: FPoint = undefined;

    while (true) {
        point_inside_block = current_point;

        point_inside_block.plusEqualsVec(lookup_displacement);

        const tile = map.findWallAtPoint(point_inside_block);

        if (tile != 0) {
            result.tile = tile;
            break;
        }

        if (current_point.x < 0 or current_point.x > SCREEN_WIDTH or
            current_point.y < 0 or current_point.y > SCREEN_HEIGHT) {
            result.tile = null;
            break;
        }

        if (callback) |callback2| {
            const function = callback2.@"fn";
            function(&current_point, callback2.data);
        }

        current_point.plusEqualsVec(advancement);
    }

    result.result_point = current_point;
}

