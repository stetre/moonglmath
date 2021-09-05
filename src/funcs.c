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
 | glmath functions                                                             |
 *------------------------------------------------------------------------------*/

#define badarg(L, arg) luaL_argerror((L), (arg), "invalid argument type")

static int Det(lua_State *L)
    {
    if(ismat(L,1)) return mat_Det(L);
    return badarg(L, 1);
    }

static int Adj(lua_State *L)
    {
    if(ismat(L,1)) return mat_Adj(L);
    return badarg(L, 1);
    }

static int Inv(lua_State *L)
    {
    if(ismat(L,1)) return mat_Inv(L);
    if(isquat(L,1)) return quat_Inv(L);
    if(iscomplex(L,1)) return complex_Inv(L);
    return badarg(L, 1);
    }


static int Norm(lua_State *L)
    {
    if(isvec(L,1)) return vec_Norm(L);
    if(isquat(L,1)) return quat_Norm(L);
    if(iscomplex(L,1)) return complex_Norm(L);
    return badarg(L, 1);
    }

static int Norm2(lua_State *L)
    {
    if(isvec(L,1)) return vec_Norm2(L);
    if(isquat(L,1)) return quat_Norm2(L);
    if(iscomplex(L,1)) return complex_Norm2(L);
    return badarg(L, 1);
    }

static int Conj(lua_State *L)
    {
    if(isquat(L,1)) return quat_Conj(L);
    if(iscomplex(L,1)) return complex_Conj(L);
    return badarg(L, 1);
    }

static int Parts(lua_State *L)
    {
    if(isquat(L,1)) return quat_Parts(L);
    if(iscomplex(L,1)) return complex_Parts(L);
    return badarg(L, 1);
    }

static int Normalize(lua_State *L)
    {
    if(isvec(L,1)) return vec_Normalize(L);
    if(isquat(L,1)) return quat_Normalize(L);
    if(iscomplex(L,1)) return complex_Normalize(L);
    return badarg(L, 1);
    }


static int Trace(lua_State *L)
    {
    if(ismat(L,1)) return mat_Trace(L);
    return badarg(L, 1);
    }

static int Transpose(lua_State *L)
    {
    if(isvec(L,1)) return vec_Transpose(L);
    if(ismat(L,1)) return mat_Transpose(L);
    return badarg(L, 1);
    }

static int Row(lua_State *L)
    {
    if(ismat(L,1)) return mat_Row(L);
    return badarg(L, 1);
    }

static int Column(lua_State *L)
    {
    if(ismat(L,1)) return mat_Column(L);
    return badarg(L, 1);
    }

static int Clamp(lua_State *L)
    {
    if(lua_isnumber(L,1)) return num_Clamp(L);
    if(isvec(L,1)) return vec_Clamp(L);
    if(ismat(L,1)) return mat_Clamp(L);
    return badarg(L, 1);
    }

static int Mix(lua_State *L)
    {
    if(lua_isnumber(L,1)) return num_Mix(L);
    if(isvec(L,1)) return vec_Mix(L);
    if(ismat(L,1)) return mat_Mix(L);
    if(isquat(L,1)) return quat_Mix(L);
    return badarg(L, 1);
    }

static int Slerp(lua_State *L)
    {
    if(isquat(L,1)) return quat_Slerp(L);
    return badarg(L, 1);
    }

static int Step(lua_State *L)
    {
    if(lua_isnumber(L,1)) return num_Step(L);
    if(isvec(L,1)) return vec_Step(L);
    if(ismat(L,1)) return mat_Step(L);
    return badarg(L, 1);
    }

static int Smoothstep(lua_State *L)
    {
    if(lua_isnumber(L,1)) return num_Smoothstep(L);
    if(isvec(L,1)) return vec_Smoothstep(L);
    if(ismat(L,1)) return mat_Smoothstep(L);
    return badarg(L, 1);
    }

static int Fade(lua_State *L)
    {
    if(lua_isnumber(L,1)) return num_Fade(L);
    if(isvec(L,1)) return vec_Fade(L);
    if(ismat(L,1)) return mat_Fade(L);
    return badarg(L, 1);
    }

/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "adj", Adj },
        { "det", Det },
        { "inv", Inv },
        { "norm", Norm },
        { "norm2", Norm2 },
        { "conj", Conj },
        { "parts", Parts },
        { "normalize", Normalize },
        { "trace", Trace },
        { "transpose", Transpose },
        { "row", Row },
        { "column", Column },
        { "clamp", Clamp },
        { "mix", Mix },
        { "slerp", Slerp },
        { "step", Step },
        { "smoothstep", Smoothstep },
        { "fade", Fade },
        { NULL, NULL } /* sentinel */
    };

void moonglmath_open_funcs(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

