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
 | Functions acting on plain numbers (double)                                   |
 *------------------------------------------------------------------------------*/

double clamp(double x, double minval, double maxval)
    {
    if(x < minval) return minval;
    if(x > maxval) return maxval;
    return x;
    }

int num_Clamp(lua_State *L)
    {
    double x = luaL_checknumber(L, 1);
    double minval = luaL_checknumber(L, 2);
    double maxval = luaL_checknumber(L, 3);
    lua_pushnumber(L, clamp(x, minval, maxval));
    return 1;
    }

double mix(double x, double y, double k)
    {
    return x*(1.0-k) + y*k;
    }

int num_Mix(lua_State *L)
    {
    double x = luaL_checknumber(L, 1);
    double y = luaL_checknumber(L, 2);
    double k = luaL_checknumber(L, 3);
    lua_pushnumber(L, mix(x, y, k));
    return 1;
    }

double step(double x, double edge)
    {
    return x<=edge ? 0.0 : 1.0;
    }

int num_Step(lua_State *L)
    {
    double x = luaL_checknumber(L, 1);
    double edge = luaL_checknumber(L, 2);
    lua_pushnumber(L, step(x, edge));
    return 1;
    }

double smoothstep(double x, double edge0, double edge1)
    {
    double t = clamp((x-edge0)/(edge1-edge0), 0, 1);
    return t*t*(3-2*t);
    }

int num_Smoothstep(lua_State *L)
    {
    double x = luaL_checknumber(L, 1);
    double edge0 = luaL_checknumber(L, 2);
    double edge1 = luaL_checknumber(L, 3);
    lua_pushnumber(L, smoothstep(x, edge0, edge1));
    return 1;
    }

double fade(double x, double edge0, double edge1)
    {
    double t = clamp((x-edge0)/(edge1-edge0), 0, 1);
    return t*t*t*(t*(t*6-15)+10); // 6t^5 - 15t^4 + 10t^3 (Perlin's improved smoothstep)
    }

int num_Fade(lua_State *L)
    {
    double x = luaL_checknumber(L, 1);
    double edge0 = luaL_checknumber(L, 2);
    double edge1 = luaL_checknumber(L, 3);
    lua_pushnumber(L, fade(x, edge0, edge1));
    return 1;
    }

