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
 | Misc utilities                                                               |
 *------------------------------------------------------------------------------*/

int noprintf(const char *fmt, ...) 
    { (void)fmt; return 0; }

int notavailable(lua_State *L, ...) 
    { NOT_AVAILABLE; }


/*------------------------------------------------------------------------------*
 | Time utilities                                                               |
 *------------------------------------------------------------------------------*/

double now(void)
    {
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    if(clock_gettime(CLOCK_MONOTONIC,&ts)!=0)
        { printf("clock_gettime error\n"); return -1; }
    return ts.tv_sec + ts.tv_nsec*1.0e-9;
#else
    struct timeval tv;
    if(gettimeofday(&tv, NULL) != 0)
        { printf("gettimeofday error\n"); return -1; }
    return tv.tv_sec + tv.tv_usec*1.0e-6;
#endif
    }

double tstosec(const struct timespec *ts)
    {
    return ts->tv_sec*1.0+ts->tv_nsec*1.0e-9;
    }

void sectots(struct timespec *ts, double seconds)
    {
    ts->tv_sec=(time_t)seconds;
    ts->tv_nsec=(long)((seconds-((double)ts->tv_sec))*1.0e9);
    }

/*------------------------------------------------------------------------------*
 | Metatables handling                                                          |
 *------------------------------------------------------------------------------*/

int newmetatable(lua_State *L, const char *metatable)
    {
    luaL_newmetatable(L, metatable);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index"); /* metatable.__index = metatable */
    lua_pop(L, 1);
    return 0;
    }

int setmetatable(lua_State *L, const char *metatable)
/* Sets the metatable of the table on top of the stack */
    {
    luaL_getmetatable(L, metatable);
    lua_setmetatable(L, -2);
    return 0;
    }

int testmetatable(lua_State *L, int arg, const char *metatable)
/* Tests the metatable of the table at index arg */
    {
    int ok = 0;
    if(lua_getmetatable(L, arg) == 0) return 0;
    luaL_getmetatable(L, metatable);
    ok = lua_compare(L, -1, -2, LUA_OPEQ);
    lua_pop(L, 2);
    return ok;
    }

int checkmetatable(lua_State *L, int arg, const char *metatable)
/* Tests the metatable of the table at index arg */
    {
    if(!testmetatable(L, arg, metatable))
        return luaL_argerror(L, arg, lua_pushfstring(L, "%s expected", metatable));
    return 1;
    }

int metatable_setfuncs(lua_State *L, const char *metatable, const luaL_Reg *metamethods, 
            const luaL_Reg *methods)
    {
    luaL_getmetatable(L, metatable);
    if(metamethods)
        luaL_setfuncs(L, metamethods, 0);
    if(methods)
        {
        luaL_getsubtable(L, -1, "__index");
        luaL_setfuncs(L, methods, 0);   
        lua_pop(L, 1);
        }
    lua_pop(L, 1);
    return 0;
    }

int checkboolean(lua_State *L, int arg)
    {
    if(!lua_isboolean(L, arg))
        return (int)luaL_argerror(L, arg, "boolean expected");
    return lua_toboolean(L, arg) ? 1 : 0;
    }

int checkoption_hint(lua_State *L, int arg, const char *def, const char *const lst[])
/* Variant of luaL_checkoption(), with an added hint in the error message */
    {
    const char *hint = NULL;
    const char *name = (def) ? luaL_optstring(L, arg, def) : luaL_checkstring(L, arg);
    int i;
    for (i=0; lst[i]; i++)
        if (strcmp(lst[i], name) == 0)  return i;

    if(lua_checkstack(L, i*2))
        {
        for(i=0; lst[i]; i++)
            {
            lua_pushfstring(L, "'%s'", lst[i]);
            lua_pushstring(L, "|");
            }
        i = i*2;
        if(i>0)
            {
            lua_pop(L, 1); /* the last separator */
            lua_concat(L, i-1);
            hint = lua_tostring(L, -1); 
            }
        }
    if(hint)
        return luaL_argerror(L, arg, lua_pushfstring(L, 
                    "invalid option '%s', valid options are: %s", name, hint));
    return luaL_argerror(L, arg, lua_pushfstring(L, "invalid option '%s'", name));
    }

int isoption(lua_State *L, int arg, const char *const lst[])
/* Variant of luaL_checkoption(), checks only and returns a boolean */
    {
    int i;
    const char *name;

    if(!lua_isstring(L, arg)) return 0;
    name = lua_tostring(L, arg);
    for (i=0; lst[i]; i++)
        if (strcmp(lst[i], name) == 0)  return 1;
    return 0;
    }

