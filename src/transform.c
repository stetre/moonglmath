/* The MIT License (MIT)
 *
 * Copyright (c) 2016 Stefano Trettel
 *
 * Software repository: MoonGLMATH, https://github.com/stetre/moonglmath
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "internal.h"

/*------------------------------------------------------------------------------*
 | Elementary transforms                                                        |
 *------------------------------------------------------------------------------*/

void translate(mat_t m, double x, double y, double z)
/* 1 0 0 x
 * 0 1 0 y
 * 0 0 1 z
 * 0 0 0 1
 *
 * T^-1(x,y,z) = T(-x, -y, -z)
 */
    {
    mat_clear(m);
    m[0][3] = x;
    m[1][3] = y;
    m[2][3] = z;
    m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0;
    }

void scale(mat_t m, double x, double y, double z)
/* x 0 0 0
 * 0 y 0 0
 * 0 0 z 0
 * 0 0 0 1
 */
    {
    mat_clear(m);
    m[0][0] = x;
    m[1][1] = y;
    m[2][2] = z;
    m[3][3] = 1.0;
    }

static void rotate(mat_t m, double x, double y, double z, double rad)
/* c+(1-c)x^2  (1-c)xy-sz (1-c)xz+sy  0
 * (1-c)xy+sz  c+(1-c)y^2 (1-c)yz-sx  0
 * (1-c)xz-sy  (1-c)yz+sx c+(1-c)z^2  0
 *     0           0          0       1
 *
 * c=cos(rad) s=sin(rad)
 */
    {
    double s, c, c1, a, b;
    c = cos(rad);
    s = sin(rad);
    c1 = 1.0 - c;
    mat_clear(m); 
    m[0][0] = c + c1 * x*x;
    m[1][1] = c + c1 * y*y;
    m[2][2] = c + c1 * z*z;
    m[3][3] = 1;
    a = c1*x*y; b = s*z;
    m[0][1] = a - b;
    m[1][0] = a + b;
    a = c1*x*z; b = s*y;
    m[0][2] = a + b;
    m[2][0] = a - b;
    a = c1*y*z; b = s*x;
    m[1][2] = a - b;
    m[2][1] = a + b;
    }


void rotate_x(mat_t m, double rad)
/*  1  0  0  0
 *  0  c -s  0
 *  0  s  c  0
 *  0  0  0  1
 */
    {
    double c = cos(rad);
    double s = sin(rad);
    mat_clear(m);
    m[0][0] = m[3][3] = 1;
    m[1][1] = m[2][2] = c;
    m[1][2] = -s;
    m[2][1] = s;
    }

void rotate_y(mat_t m, double rad)
/*  c  0  s  0
 *  0  1  0  0
 * -s  0  c  0
 *  0  0  0  1
 */
    {
    double c = cos(rad);
    double s = sin(rad);
    mat_clear(m);
    m[1][1] = m[3][3] = 1;
    m[0][0] = m[2][2] = c;
    m[2][0] = -s;
    m[0][2] = s;
    }

void rotate_z(mat_t m, double rad)
/*  c -s  0  0
 *  s  c  0  0
 *  0  0  1  0
 *  0  0  0  1
 */
    {
    double c = cos(rad);
    double s = sin(rad);
    mat_clear(m);
    m[0][0] = m[1][1] = c;
    m[2][2] = m[3][3] = 1;
    m[0][1] = -s;
    m[1][0] = s;
    }


static int Translate(lua_State *L)
    {
    vec_t v;
    mat_t m;
    if(!testvec(L, 1, v, NULL, NULL)) 
        {
        v[0] = luaL_checknumber(L, 1);
        v[1] = luaL_checknumber(L, 2);
        v[2] = luaL_checknumber(L, 3);
        }
    translate(m, v[0], v[1], v[2]);
    return pushmat(L, m, 4, 4, 4, 4);
    }

static int Scale(lua_State *L)
    {
    vec_t v;
    mat_t m;
    if(!testvec(L, 1, v, NULL, NULL)) 
        {
        v[0] = luaL_checknumber(L, 1);
        if(lua_isnoneornil(L, 2) && lua_isnoneornil(L, 3))
            v[1] = v[2] = v[0];
        else
            {
            v[1] = luaL_checknumber(L, 2);
            v[2] = luaL_checknumber(L, 3);
            }
        }
    scale(m, v[0], v[1], v[2]);
    return pushmat(L, m, 4, 4, 4, 4);
    }


static int Rotate(lua_State *L)
    {
    vec_t v;
    mat_t m;
    double rad = luaL_checknumber(L, 1);
    if(!testvec(L, 2, v, NULL, NULL)) 
        {
        v[0] = luaL_checknumber(L, 2);
        v[1] = luaL_checknumber(L, 3);
        v[2] = luaL_checknumber(L, 4);
        }
    rotate(m, v[0], v[1], v[2], rad);
    return pushmat(L, m, 4, 4, 4, 4);
    }


static int RotateX(lua_State *L)
    {
    mat_t m;
    double rad = luaL_checknumber(L, 1);
    rotate_x(m, rad);
    return pushmat(L, m, 4, 4, 4, 4);
    }

static int RotateY(lua_State *L)
    {
    mat_t m;
    double rad = luaL_checknumber(L, 1);
    rotate_y(m, rad);
    return pushmat(L, m, 4, 4, 4, 4);
    }

static int RotateZ(lua_State *L)
    {
    mat_t m;
    double rad = luaL_checknumber(L, 1);
    rotate_z(m, rad);
    return pushmat(L, m, 4, 4, 4, 4);
    }

/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "translate", Translate },
        { "scale", Scale },
        { "rotate", Rotate },
        { "rotate_x", RotateX },
        { "rotate_y", RotateY },
        { "rotate_z", RotateZ },
        { NULL, NULL } /* sentinel */
    };

void moonglmath_open_transform(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

