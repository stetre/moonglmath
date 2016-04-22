-- MoonGLMATH example: quaternions.lua

glmath = require("moonglmath")

q1 = glmath.quat(1, 2, 3, 4)
print(q1)

q2 = glmath.quat(4, 3, 2, 1)
print(q2)


print("scalar and vector parts", q1, q1:parts())
print("unary minus", q1, -q1)
print("addition", q1, q2, q1+q2)
print("subtraction", q1, q2, q1-q2)
print("multiplication", q1, q2, q1*q2)
print("integer power", q1, q1^3, q1*q1*q1)
print("norm", q1, q1:norm()) -- also glmath.norm(q1)
print("conjugate", q1, q1:conj()) -- also glmath.conj(q1)
print("inverse", q1, q1:inv()) -- q^-1 would do also
print("multiplication by a scalar", q1, 2*q1, q1*2)
print("division by a scalar", q1, q1/2)
print("normalization", q1, q1:normalize()) -- glmath.normalize(q1)
print("3x3 rotation matrix", q1, q1:mat3())
print("4x4 rotation matrix", q1, q1:mat4())

