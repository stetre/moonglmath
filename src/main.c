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

static lua_State *moonglmath_L = NULL;

static void AtExit(void)
    {
    if(moonglmath_L)
        {
        enums_free_all(moonglmath_L);
        moonglmath_L = NULL;
        }
    }

static int AddVersions(lua_State *L)
/* Add version strings to the gl table */
    {
    lua_pushstring(L, "_VERSION");
    lua_pushstring(L, "MoonGLMATH "MOONGLMATH_VERSION);
    lua_settable(L, -3);
    return 0;
    }

int luaopen_moonglmath(lua_State *L)
/* Lua calls this function to load the module */
    {
    moonglmath_L = L;
    moonglmath_utils_init(L);
    atexit(AtExit);

    lua_newtable(L); /* the gl table */
    AddVersions(L);

    /* add glmath functions: */
    moonglmath_open_enums(L);
    moonglmath_open_datahandling(L);
    moonglmath_open_tracing(L);
    moonglmath_open_vec(L);
    moonglmath_open_box(L);
    moonglmath_open_mat(L);
    moonglmath_open_quat(L);
    moonglmath_open_complex(L);
    moonglmath_open_funcs(L);
    moonglmath_open_transform(L);
    moonglmath_open_viewing(L);
    moonglmath_open_hostmem(L);

    /* Add functions implemented in Lua */
    lua_pushvalue(L, -1); lua_setglobal(L, "moonglmath");
    if(luaL_dostring(L, "require('moonglmath.vecsugar')") != 0) lua_error(L);
    if(luaL_dostring(L, "require('moonglmath.boxsugar')") != 0) lua_error(L);
    if(luaL_dostring(L, "require('moonglmath.matsugar')") != 0) lua_error(L);
    if(luaL_dostring(L, "require('moonglmath.quatsugar')") != 0) lua_error(L);
    if(luaL_dostring(L, "require('moonglmath.complexsugar')") != 0) lua_error(L);
    if(luaL_dostring(L, "require('moonglmath.utils')") != 0) lua_error(L);
    lua_pushnil(L);  lua_setglobal(L, "moonglmath");

    return 1;
    }

