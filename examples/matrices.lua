#!/usr/bin/env lua
-- MoonGLMATH example: matrices.lua

local glmath = require("moonglmath")

local m = glmath.mat3()
print(m)

m = glmath.mat2x3()
print(m)

m = glmath.mat2x3(1, 2, 3, 4)
print(m)

m = glmath.mat3x2(1, 2, 3, 4)
print(m)

m = glmath.mat3(m)
print(m)

local v1 = glmath.vec2(1, 2)
local v2 = glmath.vec2(3, 4)
m = glmath.mat2(v1, v2)
print(m)

m = glmath.mat3(v1, v2)
print(m)

v1 = glmath.vec2r(1, 2)
v2 = glmath.vec2r(3, 4)
m = glmath.mat2(v1, v2)
print(m)

m = glmath.mat3(v1, v2)
print(m)

m = glmath.mat2(1, 2, 3, 4)
print(m)

print("det", m, m:det())
print("trace", m, m:trace())
print("transpose", m, m:transpose())
print("adj", m, m:adj())
print("inv", m, m:inv(), m*m:inv())

print("unary minus", m, -m)
print("addition", m, m+m)
print("subtraction", m, m-m)
print("multiplication by a scalar", m, 2*m, m*2)
print("division by a scalar", m, m/2)
print("matrix multiplication", m, m*m)
local v = glmath.vec2(1, 2)
print("matrix-vector multiplication", m, m*v)
