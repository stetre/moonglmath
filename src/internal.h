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

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "moonglmath.h"

#define TOSTR_(x) #x
#define TOSTR(x) TOSTR_(x)

/* Note: all the dynamic symbols of this library (should) start with 'moonglmath_' .
 * The only exception is the luaopen_moonglmath() function, which is searched for
 * with that name by Lua.
 * MoonGLMATH's string references on the Lua registry also start with 'moonglmath_'.
 */

#if LUA_VERSION_NUM < 503 /* defined in lua.h */
#error "MoonGLMATH requires Lua v5.3 or greater"
#endif

#if (LUAVER != LUA_VERSION_NUM)
#pragma message ("lua.h version is "TOSTR(LUA_VERSION_NUM))
#error "Lua version mismatch"
#endif

#include "enum.h"

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
 */

/* Metatables names (keys in the Lua registry) */
#define VEC_MT "moonglmath_vec" 
#define MAT_MT "moonglmath_mat" 
#define QUAT_MT "moonglmath_quat" 

/* Internal formats */
#define vec_t moonglmath_vec_t
#define mat_t moonglmath_mat_t
#define quat_t moonglmath_quat_t
typedef lua_Number vec_t[4];
typedef lua_Number mat_t[4][4];
typedef lua_Number quat_t[4];

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
#define now moonglmath_now
double now(void);
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
#define testmetatable moonglmath_testmetatable
int testmetatable(lua_State *L, int arg, const char *metatable);
#define checkmetatable moonglmath_checkmetatable
int checkmetatable(lua_State *L, int arg, const char *metatable);
#define metatable_setfuncs moonglmath_metatable_setfuncs
int metatable_setfuncs(lua_State *L, const char *metatable, const luaL_Reg *metamethods, 
            const luaL_Reg *methods);

/* quat.c ------------------------------------------------------------------------*/
#define quat_clear(q)   memset((q), 0, sizeof(quat_t))
#define quat_copy(dst, q)   memcpy((dst), (q), sizeof(quat_t))
#define isquat(L, arg) testmetatable((L), arg, QUAT_MT)
#define testquat moonglmath_testquat
int testquat(lua_State *L, int arg, quat_t q);
#define checkquat moonglmath_checkquat
int checkquat(lua_State *L, int arg, quat_t q);
#define pushquat moonglmath_pushquat
int pushquat(lua_State *L, quat_t q);
#define quat_unm moonglmath_quat_unm
void quat_unm(quat_t dst, quat_t q);
#define quat_add moonglmath_quat_add
void quat_add(quat_t dst, quat_t q1, quat_t q2);
#define quat_sub moonglmath_quat_sub
void quat_sub(quat_t dst, quat_t q1, quat_t q2);
#define quat_norm moonglmath_quat_norm
lua_Number quat_norm(quat_t q);
#define quat_normalize moonglmath_quat_normalize
void quat_normalize(quat_t q);
#define quat_conj moonglmath_quat_conj
void quat_conj(quat_t dst, quat_t q); 
#define quat_inv moonglmath_quat_inv
void quat_inv(quat_t dst, quat_t q);
#define quat_div moonglmath_quat_div
void quat_div(quat_t dst, quat_t q, lua_Number s);
#define quat_mul moonglmath_quat_mul
void quat_mul(quat_t dst, quat_t q, quat_t p);
#define quat_qxs moonglmath_quat_qxs
void quat_qxs(quat_t dst, quat_t q, lua_Number s);

#define quat_Norm moonglmath_quat_Norm
int quat_Norm(lua_State *L);
#define quat_Normalize moonglmath_quat_Normalize
int quat_Normalize(lua_State *L);
#define quat_Inv moonglmath_quat_Inv
int quat_Inv(lua_State *L);
#define quat_Conj moonglmath_quat_Conj
int quat_Conj(lua_State *L);
#define quat_FromMat moonglmath_quat_FromMat
int quat_FromMat(lua_State *L);

/* vec.c ------------------------------------------------------------------------*/
#define vec_clear(v)    memset((v), 0, sizeof(vec_t))
#define checkvecsize(L, sz) do {                        \
    if(((sz)<1) || ((sz)>4))                            \
        return luaL_error(L, "invalid vector size");    \
} while(0)
#define isvec(L, arg) testmetatable((L), arg, VEC_MT)
#define testvec moonglmath_testvec
int testvec(lua_State *L, int arg, vec_t v, size_t *size, unsigned int *isrow);
#define checkvec moonglmath_checkvec
int checkvec(lua_State *L, int arg, vec_t v, size_t *size, unsigned int *isrow);
#define pushvec moonglmath_pushvec
int pushvec(lua_State *L, vec_t v, size_t vsize, size_t size, unsigned int isrow);
#define vec_unm moonglmath_vec_unm
void vec_unm(vec_t dst, vec_t v, size_t n);
#define vec_add moonglmath_vec_add
void vec_add(vec_t dst, vec_t v1, vec_t v2, size_t n);
#define vec_sub moonglmath_vec_sub
void vec_sub(vec_t dst, vec_t v1, vec_t v2, size_t n);
#define vec_norm moonglmath_vec_norm
lua_Number vec_norm(vec_t v, size_t n);
#define vec_normalize moonglmath_vec_normalize
void vec_normalize(vec_t v, size_t n);
#define vec_div moonglmath_vec_div
void vec_div(vec_t dst, vec_t v, lua_Number s, size_t n);
#define vec_dot moonglmath_vec_dot
lua_Number vec_dot(vec_t v1, vec_t v2, size_t n);
#define vec_vxs moonglmath_vec_vxs
void vec_vxs(vec_t dst, vec_t v, lua_Number s, size_t n);
#define vec_vxv moonglmath_vec_vxv
void vec_vxv(mat_t dst, vec_t v1, vec_t v2, size_t n);
#define vec_cross moonglmath_vec_cross
void vec_cross(vec_t dst, vec_t v1, vec_t v2);

#define vec_Norm moonglmath_vec_Norm
int vec_Norm(lua_State *L);
#define vec_Normalize moonglmath_vec_Normalize
int vec_Normalize(lua_State *L);
#define vec_Transpose moonglmath_vec_Transpose
int vec_Transpose(lua_State *L);

/* mat.c ------------------------------------------------------------------------*/
#define mat_clear(m)        memset((m), 0, sizeof(mat_t))
#define mat_copy(dst,m)     memcpy((dst), (m), sizeof(mat_t))
#define checkmatsize(L, nr, nc) do {                    \
    if(((nr)<1) || ((nr)>4) || ((nc)<1) || ((nc)>4))    \
        return luaL_error(L, "invalid matrix size");    \
} while(0)
#define ismat(L, arg) testmetatable((L), arg, MAT_MT)
#define testmat moonglmath_testmat
int testmat(lua_State *L, int arg, mat_t m, size_t *nr, size_t *nc);
#define checkmat moonglmath_checkmat
int checkmat(lua_State *L, int arg, mat_t m, size_t *nr, size_t *nc);
#define pushmat moonglmath_pushmat
int pushmat(lua_State *L, mat_t m, size_t mr, size_t mc, size_t nr, size_t nc);
#define mat_unm moonglmath_mat_unm
void mat_unm(mat_t dst, mat_t m, size_t nr, size_t nc);
#define mat_transpose moonglmath_mat_transpose
void mat_transpose(mat_t dst, mat_t m, size_t nr, size_t nc);
#define mat_add moonglmath_mat_add
void mat_add(mat_t dst, mat_t m1, mat_t m2, size_t nr, size_t nc);
#define mat_sub moonglmath_mat_sub
void mat_sub(mat_t dst, mat_t m1, mat_t m2, size_t nr, size_t nc);
#define mat_div moonglmath_mat_div
void mat_div(mat_t dst, mat_t m, lua_Number s, size_t nr, size_t nc);
#define mat_mul moonglmath_mat_mul
void mat_mul(mat_t dst, mat_t m1, mat_t m2, size_t nr1, size_t nc1, size_t nc2);
#define mat_mulby moonglmath_mat_mulby
void mat_mulby(mat_t m1, mat_t m2, size_t nr1, size_t nc1, size_t nc2);
#define mat_mxs moonglmath_mat_mxs
void mat_mxs(mat_t dst, mat_t m, lua_Number s, size_t nr, size_t nc);
#define mat_mxv moonglmath_mat_mxv
void mat_mxv(vec_t dst, mat_t m, vec_t v, size_t nr, size_t nc);
#define mat_vxm moonglmath_mat_vxm
void mat_vxm(vec_t dst, vec_t v, mat_t m, size_t nr, size_t nc);
#define mat_det2 moonglmath_mat_det2
lua_Number mat_det2(mat_t m);
#define mat_det3 moonglmath_mat_det3
lua_Number mat_det3(mat_t m);
#define mat_det4 moonglmath_mat_det4
lua_Number mat_det4(mat_t m);
#define mat_adj moonglmath_mat_adj
void mat_adj(mat_t dst, mat_t m, size_t n);
#define mat_inv moonglmath_mat_inv
int mat_inv(mat_t dst, mat_t m, size_t n);

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

/* transform.c -------------------------------------------------------------*/
#define translate moonglmath_translate
void translate(mat_t m, lua_Number x, lua_Number y, lua_Number z);
#define scale moonglmath_scale
void scale(mat_t m, lua_Number x, lua_Number y, lua_Number z);
#define rotate_x moonglmath_rotate_x
void rotate_x(mat_t m, lua_Number rad);
#define rotate_y moonglmath_rotate_y
void rotate_y(mat_t m, lua_Number rad);
#define rotate_z moonglmath_rotate_z
void rotate_z(mat_t m, lua_Number rad);

/* main.c */
int luaopen_moonglmath(lua_State *L);
void moonglmath_open_mat(lua_State *L);
void moonglmath_open_vec(lua_State *L);
void moonglmath_open_quat(lua_State *L);
void moonglmath_open_transform(lua_State *L);
void moonglmath_open_viewing(lua_State *L);
void moonglmath_open_funcs(lua_State *L);

/*------------------------------------------------------------------------------*
 | Debug and other utilities                                                    |
 *------------------------------------------------------------------------------*/

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

#define since(t) (now() - (t))
#define tstosec moonglmath_tstosec
double tstosec(const struct timespec *ts);
#define sectots moonglmath_sectots
void sectots(struct timespec *ts, double seconds);
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
