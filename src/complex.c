/* The MIT License (MIT)
 *
 * Copyright (c) 2018 Stefano Trettel
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

int testcomplex(lua_State *L, int arg, complex_t *z)
/* Tests if the element at arg is a complex number and sets z accordingly
 * (unless it is passed as 0).
 * Accepts also a real number x, in which case z = x+I*0;
 */
    {
    double x, y;
    if(lua_isnumber(L, arg))
        {
        if(z != NULL)
            *z = lua_tonumber(L, arg);
        return 1;
        }

    if(!testmetatable(L, arg, COMPLEX_MT)) return 0;

    if(z != NULL)
        {
        lua_geti(L, arg, 1);
        x = luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_geti(L, arg, 2);
        y = luaL_checknumber(L, -1);
        lua_pop(L, 1);
        *z = x + I*y;
        }
    return 1;
    }

int checkcomplex(lua_State *L, int arg, complex_t *z)
/* Same as testcomplex(), but raises an error if the test fails.
 */
    {
    if(!testcomplex(L, arg, z))
        return luaL_argerror(L, arg, lua_pushfstring(L, "%s expected", COMPLEX_MT));
    return 1;
    }


int pushcomplex(lua_State *L, complex_t z)
    {
    lua_newtable(L);
    setmetatable(L, COMPLEX_MT);
    lua_pushnumber(L, creal(z));
    lua_seti(L, -2, 1);
    lua_pushnumber(L, cimag(z));
    lua_seti(L, -2, 2);
    return 1;
    }

/*------------------------------------------------------------------------------*
 |                                                                              |
 *------------------------------------------------------------------------------*/

static int ComplexArray(lua_State *L)
    {
    double x, y;
    int i, j, tbl;
    int n = luaL_len(L, 1);
    lua_newtable(L);
    tbl = lua_gettop(L);
    j = 0;
    for(i=0; i < n; )
        {
        lua_rawgeti(L, 1, ++i);
        x = luaL_optnumber(L, -1, 0);
        lua_rawgeti(L, 1, ++i);
        y = luaL_optnumber(L, -1, 0);
        lua_pop(L, 2);
        pushcomplex(L, x + I*y);
        lua_rawseti(L, tbl, ++j);
        }
    return 1;
    }

static int Complex(lua_State *L)
/* creates a complex number from another on or from its real and imag parts
 * complex()      --> 0+0i
 * complex(x, y)  --> x+yi
 * complex(z)     --> z
 * complex({x1, y1, x2, y2, ...}) --> {z1, z2, ...}
 */
    {
    complex_t z;
    double x, y;
    int t = lua_type(L, 1);

    if((t == LUA_TNONE)||(t == LUA_TNIL)||(t == LUA_TNUMBER))
        {
        x = luaL_optnumber(L, 1, 0);
        y = luaL_optnumber(L, 2, 0);
        z = x + I*y;
        }
    else if(t == LUA_TTABLE && !testmetatable(L, 1, COMPLEX_MT))
        return ComplexArray(L);
    else
        checkcomplex(L, 1, &z);
    return pushcomplex(L, z);
    }
    
static int IsComplex(lua_State *L)
    {
    if(!testcomplex(L, 1, NULL))
        lua_pushboolean(L, 0);
    else
        lua_pushboolean(L, 1);
    return 1;
    }
    
/*------------------------------------------------------------------------------*
 | ToString                                                                     |
 *------------------------------------------------------------------------------*/

static int ToString_(lua_State *L, complex_t z)
/* With snprintf because lua_pushfstring does not support the %g conversion specifier */
    {
#define SZ 512
#define space (SZ-p)
    char str[SZ];
    int p=0;
    if(cimag(z)>=0)
        p = snprintf(str, space, ""FMT"+"FMT"i", creal(z), cimag(z));
    else
        p = snprintf(str, space, ""FMT""FMT"i", creal(z), cimag(z));
    lua_pushlstring(L, str, p);
    return 1;
#undef space
#undef SZ
    }

static int ToString(lua_State *L)
    {
    complex_t z;
    checkcomplex(L, 1, &z);
    return ToString_(L, z);
    }


static int Concat(lua_State *L)
    {
    complex_t z;
    if(testcomplex(L, 1, &z))
        {
        ToString_(L, z);
        lua_pushvalue(L, 2);
        }
    else if(testcomplex(L, 2, &z))
        {
        lua_pushvalue(L, 1);
        ToString_(L, z);
        }
    else 
        return unexpected(L);
    lua_concat(L, 2);
    return 1;
    }

/*------------------------------------------------------------------------------*
 | C99 complex.h functions                                                      |
 *------------------------------------------------------------------------------*/

#define FUNC(Func, what)                \
static int Func(lua_State *L)           \
    {                                   \
    complex_t z;                        \
    checkcomplex(L, 1, &z);             \
    lua_pushnumber(L, what(z));         \
    return 1;                           \
    }

FUNC(Carg, carg)
FUNC(Cimag, cimag)
FUNC(Creal, creal)

#undef FUNC

#define FUNC(Func, what)                \
static int Func(lua_State *L)           \
    {                                   \
    complex_t z;                        \
    checkcomplex(L, 1, &z);             \
    return pushcomplex(L, what(z));     \
    }

FUNC(Csin, csin)
FUNC(Ccos, ccos)
FUNC(Ctan, ctan)
FUNC(Casin, casin)
FUNC(Cacos, cacos)
FUNC(Catan, catan)
FUNC(Csinh, csinh)
FUNC(Ccosh, ccosh)
FUNC(Ctanh, ctanh)
FUNC(Casinh, casinh)
FUNC(Cacosh, cacosh)
FUNC(Catanh, catanh)
FUNC(Cproj, cproj)
FUNC(Clog, clog)
FUNC(Cexp, cexp)
FUNC(Csqrt, csqrt)

#undef FUNC

/*------------------------------------------------------------------------------*
 | Metamethods                                                                  |
 *------------------------------------------------------------------------------*/

static int Unm(lua_State *L)
    {
    complex_t z; 
    checkcomplex(L, 1, &z);
    return pushcomplex(L, -z);
    }

#define FUNC(Func, what)                \
static int Func(lua_State *L)           \
    {                                   \
    complex_t z, z1;                    \
    checkcomplex(L, 1, &z);             \
    checkcomplex(L, 2, &z1);            \
    return pushcomplex(L, z what z1);   \
    }

FUNC(Add, +)
FUNC(Sub, -)
FUNC(Mul, *)
FUNC(Div, /)

#undef FUNC

static int Pow(lua_State *L)
    {
    complex_t z, z1;
    checkcomplex(L, 1, &z);
    checkcomplex(L, 2, &z1);
    return pushcomplex(L, cpow(z, z1));
    }


int complex_Norm(lua_State *L)
    {
    complex_t z;
    checkcomplex(L, 1, &z);
    lua_pushnumber(L, cabs(z));
    return 1;
    }

int complex_Norm2(lua_State *L)
    {
    complex_t z;
    double r; 
    checkcomplex(L, 1, &z);
    r = cabs(z);
    lua_pushnumber(L, r*r);
    return 1;
    }

int complex_Normalize(lua_State *L)
    {
    complex_t z; 
    checkcomplex(L, 1, &z);
    return pushcomplex(L, z/cabs(z));
    }

int complex_Inv(lua_State *L)
    {
    complex_t z;
    checkcomplex(L, 1, &z);
    return pushcomplex(L, 1/z);
    }

int complex_Parts(lua_State *L)
    {
    complex_t z; 
    checkcomplex(L, 1, &z);
    lua_pushnumber(L, creal(z));
    lua_pushnumber(L, cimag(z));
    return 2;
    }

int complex_Conj(lua_State *L)
    {
    complex_t z; 
    checkcomplex(L, 1, &z);
    return pushcomplex(L, conj(z));
    }

static const struct luaL_Reg Metamethods[] = 
    {
        { "__tostring", ToString },
        { "__concat", Concat },
        { "__unm", Unm },
        { "__add", Add },
        { "__sub", Sub },
        { "__mul", Mul },
        { "__div", Div },
        { "__pow", Pow },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Methods[] = 
    {
        { "parts", complex_Parts },
        { "conj", complex_Conj },
        { "norm", complex_Norm },
        { "norm2", complex_Norm2 },
        { "normalize", complex_Normalize },
        { "inv", complex_Inv },
        { "cabs", complex_Norm },
        { "cpow", Pow },
        { "carg", Carg },
        { "cimag", Cimag },
        { "creal", Creal },
        { "csin", Csin },
        { "ccos", Ccos },
        { "ctan", Ctan },
        { "casin", Casin },
        { "cacos", Cacos },
        { "catan", Catan },
        { "csinh", Csinh },
        { "ccosh", Ccosh },
        { "ctanh", Ctanh },
        { "casinh", Casinh },
        { "cacosh", Cacosh },
        { "catanh", Catanh },
        { "cproj", Cproj },
        { "clog", Clog },
        { "cexp", Cexp },
        { "csqrt", Csqrt },
        { NULL, NULL } /* sentinel */
    };



/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "complex", Complex },
        { "iscomplex", IsComplex },
//      { "conj", complex_Conj }, --> in funcs.c
        { "cabs", complex_Norm },
        { "cpow", Pow },
        { "carg", Carg },
        { "cimag", Cimag },
        { "creal", Creal },
        { "csin", Csin },
        { "ccos", Ccos },
        { "ctan", Ctan },
        { "casin", Casin },
        { "cacos", Cacos },
        { "catan", Catan },
        { "csinh", Csinh },
        { "ccosh", Ccosh },
        { "ctanh", Ctanh },
        { "casinh", Casinh },
        { "cacosh", Cacosh },
        { "catanh", Catanh },
        { "cproj", Cproj },
        { "clog", Clog },
        { "cexp", Cexp },
        { "csqrt", Csqrt },
        { NULL, NULL } /* sentinel */
    };

void moonglmath_open_complex(lua_State *L)
    {
    newmetatable(L, COMPLEX_MT);
    metatable_setfuncs(L, COMPLEX_MT, Metamethods, Methods);
    luaL_setfuncs(L, Functions, 0);
    }

