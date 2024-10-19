# ScreenMelt

A stupid Windows application made in C++ to try out the CLion IDE which creates a screen effect that melts your screen.
Nothing else.

## Compiling

I am using MinGW (g++, ninja) for this project, but MSVC will probably work too. Here's instructions for MinGW:

```shell
cmake -DCMAKE_BUILD_TYPE=Debug -G Ninja -S . -B cmake-build-debug
cmake --build cmake-build-debug
```
