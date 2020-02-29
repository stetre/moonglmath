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

int testbox(lua_State *L, int arg, box_t b, size_t *dim)
/* Tests if the element at arg is a box and sets b and *dim accordingly
 * (unless they are passed as 0).
 */
    {
    size_t i;
    size_t dim_;
    if(!testmetatable(L, arg, BOX_MT)) return 0;

    lua_getfield(L, arg, "dimensions");   
    dim_ = luaL_checkinteger(L, -1);
    lua_pop(L, 1);
    checkboxdim(L, dim_);

    if(dim != NULL) *dim = dim_;
    if(b != NULL)
        {
        box_clear(b);
        for(i=0; i < 2*dim_; i++)
            {
            lua_geti(L, arg, i+1);
            b[i] = luaL_checknumber(L, -1);
            lua_pop(L, 1);
            }
        }
    return 1;
    }

int checkbox(lua_State *L, int arg, box_t b, size_t *dim)
/* Same as testbox(), but raises a error if the test fails.
 */
    {
    if(!testbox(L, arg, b, dim))
        return luaL_argerror(L, arg, lua_pushfstring(L, "%s expected", BOX_MT));
    return 1;
    }


int pushbox(lua_State *L, box_t b, size_t dim)
/* dim = 2 or 3
 */
    {
    size_t i;
    checkboxdim(L, dim);
    lua_newtable(L);
    setmetatable(L, BOX_MT);
    for(i=0; i<2*dim; i++)
        {
        lua_pushnumber(L, b[i]);
        lua_seti(L, -2, i+1);
        }
    lua_pushinteger(L, dim);
    lua_setfield(L, -2, "dimensions");
    return 1;
    }

/*------------------------------------------------------------------------------*
 | Box                                                                          |
 *------------------------------------------------------------------------------*/

static int Box(lua_State *L, size_t dim)
/* creates a box of dimensions 'dim', from a box or a list of values 
 * the size is adapted by discarding exceeding values or adding zeros
 * in lieu of missing values.
 * boxN(b)
 * boxN([x1 ,[x2,...[,x2N]]])
 */
    {
    box_t b;
    size_t bdim, i, arg;
    arg = 1;
    if(testbox(L, arg, b, &bdim))
        return pushbox(L, b, bdim);
    checkboxdim(L, dim);
    for(i = 0; i < 2*dim; i++)
        b[i] = luaL_optnumber(L, arg++, 0);
    return pushbox(L, b, dim);
    }

static int Box2(lua_State *L) { return Box(L, 2); }
static int Box3(lua_State *L) { return Box(L, 3); }

static int IsBox(lua_State *L, size_t dim)
    {
    size_t bdim;
    if(!testbox(L, 1, NULL, &bdim))
        lua_pushboolean(L, 0);
    else
        lua_pushboolean(L, (bdim==dim));
    return 1;
    }
    
static int IsBox2(lua_State *L) { return IsBox(L, 2); }
static int IsBox3(lua_State *L) { return IsBox(L, 3); }

/*------------------------------------------------------------------------------*
 | ToString                                                                     |
 *------------------------------------------------------------------------------*/

static int ToString_(lua_State *L, box_t b, size_t dim)
/* With snprintf because lua_pushfstring does not support the %g conversion specifier */
    {
#define SZ 512
#define space (SZ-p)
    char str[SZ];
    int p=0;
    switch(dim)
        {
        case 2: p = snprintf(str, space, "[ "FMT", "FMT", "FMT", "FMT" ]", 
                                                b[0], b[1], b[2], b[3]); break;
        case 3: p = snprintf(str, space, "[ "FMT", "FMT", "FMT", "FMT", "FMT", "FMT" ]", 
                                                b[0], b[1], b[2], b[3], b[4], b[5]); break;
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
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
    box_t b;
    size_t dim;
    checkbox(L, 1, b, &dim);
    return ToString_(L, b, dim);
    }


static int Concat(lua_State *L)
    {
    box_t b;
    size_t dim;
    if(testbox(L, 1, b, &dim))
        {
        ToString_(L, b, dim);
        lua_pushvalue(L, 2);
        }
    else if(testbox(L, 2, b, &dim))
        {
        lua_pushvalue(L, 1);
        ToString_(L, b, dim);
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
        { "box2", Box2 },
        { "box3", Box3 },
        { "isbox2", IsBox2 },
        { "isbox3", IsBox3 },
        { NULL, NULL } /* sentinel */
    };

void moonglmath_open_box(lua_State *L)
    {
    newmetatable(L, BOX_MT);
    metatable_setfuncs(L, BOX_MT, Metamethods, Methods);
    luaL_setfuncs(L, Functions, 0);
    }

