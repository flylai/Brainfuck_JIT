# Brainfuck JIT

Including interpreter, optimizer, and JIT compiler

# Build

```shell
mkdir build
cd build
cmake ..
cmake --build .
```

# Usage

```
Usage:
    branfuck_jit [options] <src file>
where options include:
    -jit enable jit(interpreter is default)
    -opt enable optimizer
    -h show this message
```

# Benchmark

| Command                 | Time  |
| ----------------------- | ----- |
| mandelbrot.bf           | 27s   |
| -opt mandelbrot.bf      | 12.7s |
| -jit mandelbrot.bf      | 3.8s  |
| -opt -jit mandelbrot.bf | 1.4s  |

Use `O3` option for Clang/GCC compiler.

Use `mandelbrot` as test file.

