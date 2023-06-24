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
 | Check and push                                                               |
 *------------------------------------------------------------------------------*/

int testrect(lua_State *L, int arg, rect_t r)
/* Tests if the element at arg is a rect and sets r accordingly */
    {
    size_t i;
    if(!testmetatable(L, arg, RECT_MT)) return 0;

    if(r != NULL)
        {
        rect_clear(r);
        for(i=0; i < 4; i++)
            {
            lua_geti(L, arg, i+1);
            r[i] = luaL_checknumber(L, -1);
            lua_pop(L, 1);
            }
        }
    return 1;
    }

int checkrect(lua_State *L, int arg, rect_t r)
/* Same as testrect(), but raises a error if the test fails.
 */
    {
    if(!testrect(L, arg, r))
        return luaL_argerror(L, arg, lua_pushfstring(L, "%s expected", RECT_MT));
    return 1;
    }

int pushrect(lua_State *L, rect_t r)
    {
    size_t i;
    lua_newtable(L);
    setmetatable(L, RECT_MT);
    for(i=0; i<4; i++)
        {
        lua_pushnumber(L, r[i]);
        lua_seti(L, -2, i+1);
        }
    return 1;
    }

/*------------------------------------------------------------------------------*
 | Rect                                                                          |
 *------------------------------------------------------------------------------*/

static int Rect(lua_State *L)
/* creates a rect from another rect or a list of values 
 * the size is adapted by discarding exceeding values or adding zeros
 * in lieu of missing values.
 */
    {
    rect_t r;
    size_t i, arg;
    arg = 1;
    if(testrect(L, arg, r))
        return pushrect(L, r);
    for(i = 0; i < 4; i++)
        r[i] = luaL_optnumber(L, arg++, 0);
    return pushrect(L, r);
    }

static int IsRect(lua_State *L)
    {
    lua_pushboolean(L, testrect(L, 1, NULL));
    return 1;
    }
    
/*------------------------------------------------------------------------------*
 | ToString                                                                     |
 *------------------------------------------------------------------------------*/

static int ToString_(lua_State *L, rect_t r)
/* With snprintf because lua_pushfstring does not support the %g conversion specifier */
    {
#define SZ 512
#define space (SZ-p)
    char str[SZ];
    int p=0;
    p = snprintf(str, space, "[ "FMT", "FMT", "FMT", "FMT" ]", r[0], r[1], r[2], r[3]);
    if(space<0)
        { 
        /* truncate */
        p = SZ - 5; 
        p+=snprintf(str+p, space, " ...");
        lua_pushlstring(L, str, p);
        return 1;
        }
    lua_pushlstring(L, str, p);
    return 1;
#undef space
#undef SZ
    }

static int ToString(lua_State *L)
    {
    rect_t r;
    checkrect(L, 1, r);
    return ToString_(L, r);
    }


static int Concat(lua_State *L)
    {
    rect_t r;
    if(testrect(L, 1, r))
        {
        ToString_(L, r);
        lua_pushvalue(L, 2);
        }
    else if(testrect(L, 2, r))
        {
        lua_pushvalue(L, 1);
        ToString_(L, r);
        }
    else 
        return unexpected(L);
    lua_concat(L, 2);
    return 1;
    }


/*------------------------------------------------------------------------------*
 | Non-Lua functions (for internal use)                                         |
 *------------------------------------------------------------------------------*/

//@@TBD

/*------------------------------------------------------------------------------*
 | Metamethods                                                                  |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Metamethods[] = 
    {
        { "__tostring", ToString },
        { "__concat", Concat },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Methods[] = 
    {
        { NULL, NULL } /* sentinel */
    };


/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "rect", Rect },
        { "isrect", IsRect },
        { NULL, NULL } /* sentinel */
    };

void moonglmath_open_rect(lua_State *L)
    {
    newmetatable(L, RECT_MT);
    metatable_setfuncs(L, RECT_MT, Metamethods, Methods);
    luaL_setfuncs(L, Functions, 0);
    }

