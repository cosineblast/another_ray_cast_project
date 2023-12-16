
# what is this

This is just a simple project for testing a little bit of ray casting with SDL2.

# Building The Project

You will need the SDL2 and SDL2_image libraries in your system.
You can use either CMake or zig (0.11.0) to build the project.

## CMake

``` sh
mkdir build

cd build

cmake ..

cmake --build .
```

It will produce an executable called `main`.

## Zig

To build the executable with zig, just use `zig build`, the `main` executable with be available at `zig-out/bin/main`.

## Nix

If you build this program using nix,
you may have to use [`nixGL`](https://github.com/nix-community/nixGL) to run the executable:

```
nixGL ./main
# or wih zig run
nixGL zig build ./main
```

