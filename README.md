## MoonGLMATH: Math library for MoonGL

MoonGLMATH is a Lua math library for [MoonGL](https://github.com/stetre/moongl).

It runs on GNU/Linux and on Windows (MSYS2/MinGW-w64) and requires [Lua](http://www.lua.org/) (>=5.3).

_Authored by:_ _[Stefano Trettel](https://www.linkedin.com/in/stetre)_

[![Lua logo](./doc/powered-by-lua.gif)](http://www.lua.org/)

#### License

MIT/X11 license (same as Lua). See [LICENSE](./LICENSE).

#### Documentation

See the [Reference Manual](https://stetre.github.io/moonglmath/doc/index.html).

#### Getting and installing

Setup the build environment as described [here](https://github.com/stetre/moonlibs), then:

```sh
$ git clone https://github.com/stetre/moonglmath
$ cd moonglmath
moonglmath$ make
moonglmath$ make install # or 'sudo make install' (Ubuntu)
```

#### Example

The example below creates a few vectors and matrices and performs some operations
on them, just to give a taste of MoonGLMATH.

Other examples can be found in the **examples/** directory contained in the release package.

```lua
-- Script: hello.lua

glmath = require("moonglmath")

-- create a few size-3 vectors and 3x3 matrices:
u = glmath.vec3(2.3, 1.5, math.pi)
v = glmath.vec3(1, 2, 3)
A = glmath.mat3(3, 1, 5, 2, 8, 7, 9, 4, 6)
I = glmath.mat3() -- identity

s = u * v    -- dot product
w = u % v    -- cross product
s = A:det()  -- determinant
B = A^-1     -- matrix inverse
B = A:inv()  -- matrix inverse
A = B * A    -- matrix multiplication
I = A^-1 * A -- matrix multiplication
w = A * v    -- matrix*vector

print(u[1], u[2], u[3]) -- vector components
print(u.x, u.y, u.z) -- position syntactic sugar
print(u.r, u.g, u.b) -- color syntactic sugar
print(u.s, u.t, u.p) -- texture syntactic sugar

```

The script can be executed at the shell prompt with the standard Lua interpreter:

```shell
$ lua hello.lua
```

#### See also

* [MoonLibs - Graphics and Audio Lua Libraries](https://github.com/stetre/moonlibs).

