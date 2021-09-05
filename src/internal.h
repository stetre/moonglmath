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

/********************************************************************************
 * Internal common header                                                       *
 ********************************************************************************/

#ifndef internalDEFINED
#define internalDEFINED

#ifdef LINUX
#define _ISOC11_SOURCE /* see man aligned_alloc(3) */
#endif
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "moonglmath_local.h"

#define TOSTR_(x) #x
#define TOSTR(x) TOSTR_(x)

/* Note: all the dynamic symbols of this library (should) start with 'moonglmath_' .
 * The only exception is the luaopen_moonglmath() function, which is searched for
 * with that name by Lua.
 * MoonGLMATH's string references on the Lua registry also start with 'moonglmath_'.
 */

#include "tree.h"
#include "objects.h"
#include "enums.h"

/* A VECTOR is implemented as a table, with the elements in the array part:
 * v[i] = i-th element, i=1..N
 * vec.size = N (no. of elements)
 * vec.type = 'row' (1xN) or 'column' (Nx1)
 *
 * eg: v = { 10, 20, 30, size=3, type='column' }
 *
 * A MATRIX is implemented as a table of tables, where the tables in the
 * array part are the rows (i.e., row-major).
 * mat[i][j] = (i,j)-th element, i=1..N, j=1..M
 * mat.rows = N (no. of rows)
 * mat.columns = M (no. of columns)
 *
 * e.g. m = { { 11, 12, 13 }, { 14, 15, 16 }, rows=2, columns=3 }
 *
 * A QUATERNION is implemented as a table, with the elements in the array part:
 * w, x, y, z = q[1], q[2], q[3], q[4]
 *
 * A BOX is implemented as a table, with the elements in the array part
 * box = { minx, maxx, miny, maxy } (2D)
 * box = { minx, maxx, miny, maxy, minz, maxz } (3D)
 * box.dimensions = 2 or 3 (2D or 3D)
 */

#define FMT "%g"    // format used in __tostring()
//#define FMT "%.5f"

/*--------------------------------------------------------------------------*
 | Redefinitions                                                            |
 *--------------------------------------------------------------------------*/

#if 0
/* .c */
#define  moonglmath_
#endif

/* utils.c */
#define Malloc moonglmath_Malloc
void *Malloc(lua_State *L, size_t size);
#define MallocNoErr moonglmath_MallocNoErr
void *MallocNoErr(lua_State *L, size_t size);
#define Strdup moonglmath_Strdup
char *Strdup(lua_State *L, const char *s);
#define Free moonglmath_Free
void Free(lua_State *L, void *ptr);
#define now moonglmath_now
double now(void);
#define sleeep moonglmath_sleeep
void sleeep(double seconds);
#define since(t) (now() - (t))
#define checkoption_hint moonglmath_checkoption_hint
int checkoption_hint(lua_State *L, int arg, const char *def, const char *const lst[]);
#define isoption moonglmath_isoption
int isoption(lua_State *L, int arg, const char *const lst[]);
#define checkboolean moonglmath_checkboolean
int checkboolean(lua_State *L, int arg);
#define noprintf moonglmath_noprintf
int noprintf(const char *fmt, ...); 
#define notavailable moonglmath_notavailable
int notavailable(lua_State *L, ...);
#define newmetatable moonglmath_newmetatable
int newmetatable(lua_State *L, const char *metatable);
#define setmetatable moonglmath_setmetatable
int setmetatable(lua_State *L, const char *metatable);
#define metatable_setfuncs moonglmath_metatable_setfuncs
int metatable_setfuncs(lua_State *L, const char *metatable, const luaL_Reg *metamethods, 
            const luaL_Reg *methods);
#define checklightuserdata moonglmath_checklightuserdata
void *checklightuserdata(lua_State *L, int arg);
#define checklightuserdataorzero moonglmath_checklightuserdataorzero
void *checklightuserdataorzero(lua_State *L, int arg);
#define optlightuserdata moonglmath_optlightuserdata
void *optlightuserdata(lua_State *L, int arg);


#define checkvecsize(L, sz) do {                        \
    if(((sz)<1) || ((sz)>4))                            \
        return luaL_error(L, "invalid vector size");    \
} while(0)

#define checkboxdim(L, dim) do {                        \
    if(((dim)<2 || ((dim)>3)))                          \
        return luaL_error(L, "invalid box dimensions"); \
} while(0)

#define checkmatsize(L, nr, nc) do {                    \
    if(((nr)<1) || ((nr)>4) || ((nc)<1) || ((nc)>4))    \
        return luaL_error(L, "invalid matrix size");    \
} while(0)

/* vec.c ------------------------------------------------------------------------*/

#define vec_Norm moonglmath_vec_Norm
int vec_Norm(lua_State *L);
#define vec_Norm2 moonglmath_vec_Norm2
int vec_Norm2(lua_State *L);
#define vec_Normalize moonglmath_vec_Normalize
int vec_Normalize(lua_State *L);
#define vec_Transpose moonglmath_vec_Transpose
int vec_Transpose(lua_State *L);
#define vec_Clamp moonglmath_vec_Clamp
int vec_Clamp(lua_State *L);
#define vec_Mix moonglmath_vec_Mix
int vec_Mix(lua_State *L);
#define vec_Step moonglmath_vec_Step
int vec_Step(lua_State *L);
#define vec_Smoothstep moonglmath_vec_Smoothstep
int vec_Smoothstep(lua_State *L);
#define vec_Fade moonglmath_vec_Fade
int vec_Fade(lua_State *L);

/* box.c ------------------------------------------------------------------------*/

//@@TBD

/* mat.c ------------------------------------------------------------------------*/

#define mat_Transpose moonglmath_mat_Transpose
int mat_Transpose(lua_State *L);
#define mat_Trace moonglmath_mat_Trace
int mat_Trace(lua_State *L);
#define mat_Det moonglmath_mat_Det
int mat_Det(lua_State *L);
#define mat_Inv moonglmath_mat_Inv
int mat_Inv(lua_State *L);
#define mat_Adj moonglmath_mat_Adj
int mat_Adj(lua_State *L);
#define mat_Column moonglmath_mat_Column
int mat_Column(lua_State *L);
#define mat_Row moonglmath_mat_Row
int mat_Row(lua_State *L);
#define mat_Clamp moonglmath_mat_Clamp
int mat_Clamp(lua_State *L);
#define mat_Mix moonglmath_mat_Mix
int mat_Mix(lua_State *L);
#define mat_Step moonglmath_mat_Step
int mat_Step(lua_State *L);
#define mat_Smoothstep moonglmath_mat_Smoothstep
int mat_Smoothstep(lua_State *L);
#define mat_Fade moonglmath_mat_Fade
int mat_Fade(lua_State *L);

/* quat.c ------------------------------------------------------------------------*/
#define quat_Norm moonglmath_quat_Norm
int quat_Norm(lua_State *L);
#define quat_Norm2 moonglmath_quat_Norm2
int quat_Norm2(lua_State *L);
#define quat_Normalize moonglmath_quat_Normalize
int quat_Normalize(lua_State *L);
#define quat_Inv moonglmath_quat_Inv
int quat_Inv(lua_State *L);
#define quat_Parts moonglmath_quat_Parts
int quat_Parts(lua_State *L);
#define quat_Conj moonglmath_quat_Conj
int quat_Conj(lua_State *L);
#define quat_FromMat moonglmath_quat_FromMat
int quat_FromMat(lua_State *L);
#define quat_Mix moonglmath_quat_Mix
int quat_Mix(lua_State *L);
#define quat_Slerp moonglmath_quat_Slerp
int quat_Slerp(lua_State *L);

/* complex.c ------------------------------------------------------------------------*/
#define complex_Norm moonglmath_complex_Norm
int complex_Norm(lua_State *L);
#define complex_Norm2 moonglmath_complex_Norm2
int complex_Norm2(lua_State *L);
#define complex_Normalize moonglmath_complex_Normalize
int complex_Normalize(lua_State *L);
#define complex_Inv moonglmath_complex_Inv
int complex_Inv(lua_State *L);
#define complex_Parts moonglmath_complex_Parts
int complex_Parts(lua_State *L);
#define complex_Conj moonglmath_complex_Conj
int complex_Conj(lua_State *L);

/* num.c -------------------------------------------------------------------------*/

#define num_Clamp moonglmath_num_Clamp
int num_Clamp(lua_State *L);
#define num_Mix moonglmath_num_Mix
int num_Mix(lua_State *L);
#define num_Step moonglmath_num_Step
int num_Step(lua_State *L);
#define num_Smoothstep moonglmath_num_Smoothstep
int num_Smoothstep(lua_State *L);
#define num_Fade moonglmath_num_Fade
int num_Fade(lua_State *L);

/* datahandling.c */
#define sizeoftype moonglmath_sizeoftype
size_t sizeoftype(int type);
#define toflattable moonglmath_toflattable
int toflattable(lua_State *L, int arg);
#define testdata moonglmath_testdata
int testdata(lua_State *L, int type, size_t n, void *dst, size_t dstsize);
#define checkdata moonglmath_checkdata
int checkdata(lua_State *L, int arg, int type, void *dts, size_t dstsize);
#define pushdata moonglmath_pushdata
int pushdata(lua_State *L, int type, void *src, size_t srcsize);


/* main.c */
int luaopen_moonglmath(lua_State *L);
void moonglmath_utils_init(lua_State *L);
void moonglmath_open_datahandling(lua_State *L);
void moonglmath_open_tracing(lua_State *L);
void moonglmath_open_enums(lua_State *L);
void moonglmath_open_mat(lua_State *L);
void moonglmath_open_vec(lua_State *L);
void moonglmath_open_box(lua_State *L);
void moonglmath_open_quat(lua_State *L);
void moonglmath_open_complex(lua_State *L);
void moonglmath_open_transform(lua_State *L);
void moonglmath_open_viewing(lua_State *L);
void moonglmath_open_funcs(lua_State *L);

/*------------------------------------------------------------------------------*
 | Debug and other utilities                                                    |
 *------------------------------------------------------------------------------*/

/* Internal error codes */
#define ERR_NOTPRESENT       1
#define ERR_SUCCESS          0
#define ERR_GENERIC         -1
#define ERR_TYPE            -2
#define ERR_VALUE           -3
#define ERR_TABLE           -4
#define ERR_EMPTY           -5
#define ERR_MEMORY          -6
#define ERR_MALLOC_ZERO     -7
#define ERR_LENGTH          -8
#define ERR_POOL            -9
#define ERR_BOUNDARIES      -10
#define ERR_UNKNOWN         -11
#define errstring moonglmath_errstring
const char* errstring(int err);

/* tracing.c */
#define trace_objects moonglmath_trace_objects
extern int trace_objects;

#define badvalue(L,s)   lua_pushfstring((L), "invalid value '%s'", (s))

//#define checkoption checkoption_hint
#define checkoption luaL_checkoption

#define UNUSED noprintf /* to avoid warnings for unused parameters */

/* If this is printed, it denotes a suspect bug: */
#define UNEXPECTED_ERROR "unexpected error (%s, %d)", __FILE__, __LINE__
#define unexpected(L) luaL_error((L), UNEXPECTED_ERROR)

#define NARGS_ERROR "wrong number of arguments"
#define PERM_ERROR "operation not permitted"
/*#define OPERANDS_ERROR "invalid operands"*/
#define OPERANDS_ERROR "invalid operands (%s, %d)", __FILE__, __LINE__

#define NOT_AVAILABLE do { return luaL_error(L, "function not available"); } while(0)

#define NOT_IMPLEMENTED(func)               \
static int func(lua_State *L)               \
    { return luaL_error(L, "function "#func" is not implemented");  }

#define NOT_SUPPORTED(func)                 \
static int func(lua_State *L)               \
    { return luaL_error(L, "function "#func" is not supported"); return 0;  }


/*-----------------------------------------------------------------------*/
#if defined(DEBUG)

#define TSTART double ts = now();
#define TSTOP do {                                          \
    ts = since(ts); ts = ts*1e6;                            \
    printf("%s %d %.3f us\n", __FILE__, __LINE__, ts);      \
    ts = now();                                             \
} while(0);

#define DBG printf

#define TR() do {                                           \
    printf("trace %s %d\n",__FILE__,__LINE__);              \
} while(0)

#define BK() do {                                           \
    printf("break %s %d\n",__FILE__,__LINE__);              \
    getchar();                                              \
} while(0)

/*-----------------------------------------------------------------------*/
#else 

#define TSTART do {} while(0) 
#define TSTOP do {} while(0)    
#define DBG noprintf
#define TR()
#define BK()

/*-----------------------------------------------------------------------*/
#endif /* DEBUG */


#if 0 /* 7yy */
        { "",  },
static int (lua_State *L) //@@
    {
    CheckError(L);
    return 0;
    }

#endif

#endif /* internalDEFINED */
