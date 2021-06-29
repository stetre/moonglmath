#!/usr/bin/env lua
-- MoonGLMATH example: vectors.lua

local glmath = require("moonglmath")

local v = glmath.vec3()
print(v)

v = glmath.vec3(11, 22)
print(v)

v = glmath.vec2(v)
print(v)

v = glmath.vec4(v)
print(v)

v = glmath.vec2(1, 2, 3, 4)
print(v)

local e1 = glmath.vec3(1,0,0)
local e2 = glmath.vec3(0,1,0)
local e3 = glmath.vec3(0,0,1)
print("canonical basis", e1, e2, e3)

print("transpose", v, v:transpose())
print("norm", v, v:norm())
print("unary minus", v, -v)
print("addition", v, v+v)
print("subtraction", v, v-v)
print("multiplication by a scalar", v, 2*v, v*2)
print("division by a scalar", v, v/2)
print("dot product", v, v*v)
print("cross product", e1, e2, e1%e2)

