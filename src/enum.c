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

unsigned int enumCheck(lua_State *L, int arg, enum_t *e)
    { return e->codes[checkoption(L, arg, NULL, e->strings)]; }

int enumPush(lua_State *L, unsigned int code, enum_t *e)
    {
    size_t i;
    for(i = 0; i < e->n; i++) 
        { if(e->codes[i]==code) 
            { 
            lua_pushstring(L, e->strings[i]); 
            return 1; 
            } 
        }
    //return luaL_error(L, "invalid enum value %d", code);
    lua_pushfstring(L, "%d???", code); 
    return 1;
    }

const char* enumToString(unsigned int code, enum_t *e)
    {
    size_t i;
    for(i = 0; i < e->n; i++) 
        if(e->codes[i]==code) return e->strings[i];
    return NULL;
    }

int enumAssert(lua_State *L, enum_t *e, unsigned int code, const char *string)
    {
    const char *s = enumToString(code, e);
    if( (s==NULL) || (strcmp(s, string) !=0))
        return unexpected(L);       
    return 0;
    }

unsigned int enumOrUint(lua_State *L, int arg, unsigned int *dst, enum_t *e, unsigned int def)
/* The element at the index arg on the Lua stack may be an enum (string),
 * or an integer. In the first case the enum code is stored in 'code' and
 * 'def' is returned. In the second case the integer value is returned.
 */
    {
    if(lua_isstring(L, arg))
        {
        *dst = enumCheck(L, arg, e);
        return def;
        }
    return (unsigned int)luaL_checkinteger(L, arg);
    }

