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
 | Malloc                                                                       |
 *------------------------------------------------------------------------------*/

/* We do not use malloc(), free() etc directly. Instead, we inherit the memory
 * allocator from the main Lua state instead (see lua_getallocf in the Lua manual)
 * and use that.
 *
 * By doing so, we can use an alternative malloc() implementation without recompiling
 * this library (we have needs to recompile lua only, or execute it with LD_PRELOAD
 * set to the path to the malloc library we want to use).
 */
static lua_Alloc Alloc = NULL;
static void* AllocUd = NULL;

static void malloc_init(lua_State *L)
    {
    if(Alloc) unexpected(L);
    Alloc = lua_getallocf(L, &AllocUd);
    }

static void* Malloc_(size_t size)
    { return Alloc ? Alloc(AllocUd, NULL, 0, size) : NULL; }

static void Free_(void *ptr)
    { if(Alloc) Alloc(AllocUd, ptr, 0, 0); }

void *Malloc(lua_State *L, size_t size)
    {
    void *ptr;
    if(size == 0)
        { luaL_error(L, errstring(ERR_MALLOC_ZERO)); return NULL; }
    ptr = Malloc_(size);
    if(ptr==NULL)
        { luaL_error(L, errstring(ERR_MEMORY)); return NULL; }
    memset(ptr, 0, size);
    //DBG("Malloc %p\n", ptr);
    return ptr;
    }

void *MallocNoErr(lua_State *L, size_t size) /* do not raise errors (check the retval) */
    {
    void *ptr = Malloc_(size);
    (void)L;
    if(ptr==NULL)
        return NULL;
    memset(ptr, 0, size);
    //DBG("MallocNoErr %p\n", ptr);
    return ptr;
    }

char *Strdup(lua_State *L, const char *s)
    {
    size_t len = strnlen(s, 256);
    char *ptr = (char*)Malloc(L, len + 1);
    if(len>0)
        memcpy(ptr, s, len);
    ptr[len]='\0';
    return ptr;
    }


void Free(lua_State *L, void *ptr)
    {
    (void)L;
    //DBG("Free %p\n", ptr);
    if(ptr) Free_(ptr);
    }


/*------------------------------------------------------------------------------*
 | Time utilities                                                               |
 *------------------------------------------------------------------------------*/

#if defined(LINUX)
#include <unistd.h> /* for usleep */

#if 0
static double tstosec(const struct timespec *ts)
    {
    return ts->tv_sec*1.0+ts->tv_nsec*1.0e-9;
    }
#endif

static void sectots(struct timespec *ts, double seconds)
    {
    ts->tv_sec=(time_t)seconds;
    ts->tv_nsec=(long)((seconds-((double)ts->tv_sec))*1.0e9);
    }

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

void sleeep(double seconds)
    {
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts, ts1;
    struct timespec *req, *rem, *tmp;
    sectots(&ts, seconds);
    req = &ts;
    rem = &ts1;
    while(1)
        {
        if(nanosleep(req, rem) == 0)
            return;
        tmp = req;
        req = rem;
        rem = tmp;
        }
#else
    usleep((useconds_t)(seconds*1.0e6));
#endif
    }

#define time_init(L) do { (void)L; /* do nothing */ } while(0)

#elif defined(MINGW)

#include <windows.h>

static LARGE_INTEGER Frequency;
double now(void)
    {
    LARGE_INTEGER ts;
    QueryPerformanceCounter(&ts);
    return ((double)(ts.QuadPart))/Frequency.QuadPart;
    }

void sleeep(double seconds)
    {
    DWORD msec = (DWORD)seconds * 1000;
    //if(msec < 0) return;  DWORD seems to be unsigned
    Sleep(msec);
    }

static void time_init(lua_State *L)
    {
    (void)L;
    QueryPerformanceFrequency(&Frequency);
    }

#endif


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


/*------------------------------------------------------------------------------*
 | Light userdata                                                               |
 *------------------------------------------------------------------------------*/

void *checklightuserdata(lua_State *L, int arg)
    {
    if(lua_type(L, arg) != LUA_TLIGHTUSERDATA)
        { luaL_argerror(L, arg, "expected lightuserdata"); return NULL; }
    return lua_touserdata(L, arg);
    }

void *optlightuserdata(lua_State *L, int arg)
    {
    if(lua_isnoneornil(L, arg))
        return NULL;
    return checklightuserdata(L, arg);
    }

void *checklightuserdataorzero(lua_State *L, int arg)
    {
    int val, isnum;
    val = lua_tointegerx(L, arg, &isnum);
    if(!isnum)
        return checklightuserdata(L, arg);
    if(val != 0)
        luaL_argerror(L, arg, "expected lightuserdata or 0");
    return NULL;
    }


/*------------------------------------------------------------------------------*
 | Internal error codes                                                         |
 *------------------------------------------------------------------------------*/

const char* errstring(int err)
    {
    switch(err)
        {
        case 0: return "success";
        case ERR_GENERIC: return "generic error";
        case ERR_TABLE: return "not a table";
        case ERR_EMPTY: return "empty list";
        case ERR_TYPE: return "invalid type";
        case ERR_VALUE: return "invalid value";
        case ERR_NOTPRESENT: return "missing";
        case ERR_MEMORY: return "out of memory";
        case ERR_MALLOC_ZERO: return "zero bytes malloc";
        case ERR_LENGTH: return "invalid length";
        case ERR_POOL: return "elements are not from the same pool";
        case ERR_BOUNDARIES: return "invalid boundaries";
        case ERR_UNKNOWN: return "unknown field name";
        default:
            return "???";
        }
    return NULL; /* unreachable */
    }

/*------------------------------------------------------------------------------*
 | Inits                                                                        |
 *------------------------------------------------------------------------------*/

void moonglmath_utils_init(lua_State *L)
    {
    malloc_init(L);
    time_init(L);
    }

