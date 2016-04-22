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

static int AddVersions(lua_State *L)
/* Add version strings to the gl table */
    {
    lua_pushstring(L, "_VERSION");
    lua_pushstring(L, "MoonGLMATH "MOONGLMATH_VERSION);
    lua_settable(L, -3);
    return 0;
    }

static int GetTime(lua_State *L)
    {
    lua_pushnumber(L, moonglmath_now());
    return 1;
    }

static const struct luaL_Reg Functions[] = 
    {
        { "get_time", GetTime },
        { NULL, NULL } /* sentinel */
    };



int luaopen_moonglmath(lua_State *L)
/* Lua calls this function to load the module */
    {
    lua_newtable(L); /* the gl table */
    AddVersions(L);
    luaL_setfuncs(L, Functions, 0);

    /* add glmath functions: */
    moonglmath_open_vec(L);
    moonglmath_open_mat(L);
    moonglmath_open_quat(L);
    moonglmath_open_funcs(L);
    moonglmath_open_transform(L);
    moonglmath_open_viewing(L);
    return 1;
    }

