
## About This

This is just a simple project for testing a little bit of ray casting with SDL2.

## Building The Project

You need cmake and libSDL2 for that.

``` sh

mkdir build

cd build

cmake ..

cmake --build .
```

It will produce an executable called `main`.

## About Nix

If you build this program using the nix `SDL2` and `cmake`,
you may have to use [`nixGL`](https://github.com/nix-community/nixGL) to run the executable:

```
nixGL ./main
```

