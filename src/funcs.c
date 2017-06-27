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
    return badarg(L, 1);
    }


static int Norm(lua_State *L)
    {
    if(isvec(L,1)) return vec_Norm(L);
    if(isquat(L,1)) return quat_Norm(L);
    return badarg(L, 1);
    }

static int Conj(lua_State *L)
    {
    if(isquat(L,1)) return quat_Conj(L);
    return badarg(L, 1);
    }


static int Normalize(lua_State *L)
    {
    if(isvec(L,1)) return vec_Normalize(L);
    if(isquat(L,1)) return quat_Normalize(L);
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


/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "adj", Adj },
        { "det", Det },
        { "inv", Inv },
        { "norm", Norm },
        { "conj", Conj },
        { "normalize", Normalize },
        { "trace", Trace },
        { "transpose", Transpose },
        { "row", Row },
        { "column", Column },
        { NULL, NULL } /* sentinel */
    };

void moonglmath_open_funcs(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

