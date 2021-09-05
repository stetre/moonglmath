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

/****************************************************************************
 * MoonGLMATH library - C API                                               *
 ****************************************************************************/

#ifndef moonglmathDEFINED
#define moonglmathDEFINED

#include <complex.h>
#include <lua.h>
#include "lualib.h"
#include "lauxlib.h"
#include "compat-5.3.h"

#define MOONGLMATH_VERSION      "0.6"


/*---------------------------------------------------------------------------*
 | Types, check/test/push                                                    |
 *---------------------------------------------------------------------------*/

typedef double moonglmath_vec_t[4];
typedef double moonglmath_box_t[8];
typedef double moonglmath_mat_t[4][4];
typedef double moonglmath_quat_t[4];
typedef double complex moonglmath_complex_t;

/* Metatables names (keys in the Lua registry) */
#define MOONGLMATH_VEC_MT "moonglmath_vec"
#define MOONGLMATH_BOX_MT "moonglmath_box"
#define MOONGLMATH_MAT_MT "moonglmath_mat"
#define MOONGLMATH_QUAT_MT "moonglmath_quat"
#define MOONGLMATH_COMPLEX_MT "moonglmath_complex"

int moonglmath_testmetatable(lua_State *L, int arg, const char *metatable);
int moonglmath_checkmetatable(lua_State *L, int arg, const char *metatable);

#define moonglmath_isvec(L, arg) moonglmath_testmetatable((L), arg, MOONGLMATH_VEC_MT)
#define moonglmath_isbox(L, arg) moonglmath_testmetatable((L), arg, MOONGLMATH_BOX_MT)
#define moonglmath_ismat(L, arg) moonglmath_testmetatable((L), arg, MOONGLMATH_MAT_MT)
#define moonglmath_isquat(L, arg) moonglmath_testmetatable((L), arg, MOONGLMATH_QUAT_MT)
#define moonglmath_iscomplex(L, arg) moonglmath_testmetatable((L), arg, MOONGLMATH_COMPLEX_MT)

int moonglmath_testvec(lua_State *L, int arg, moonglmath_vec_t v, size_t *size, unsigned int *isrow);
int moonglmath_checkvec(lua_State *L, int arg, moonglmath_vec_t v, size_t *size, unsigned int *isrow);
int moonglmath_pushvec(lua_State *L, moonglmath_vec_t v, size_t vsize, size_t size, unsigned int isrow);

int moonglmath_testbox(lua_State *L, int arg, moonglmath_box_t b, size_t *dim);
int moonglmath_checkbox(lua_State *L, int arg, moonglmath_box_t b, size_t *dim);
int moonglmath_pushbox(lua_State *L, moonglmath_box_t b, size_t dim);

int moonglmath_testmat(lua_State *L, int arg, moonglmath_mat_t m, size_t *nr, size_t *nc);
int moonglmath_checkmat(lua_State *L, int arg, moonglmath_mat_t m, size_t *nr, size_t *nc);
int moonglmath_pushmat(lua_State *L, moonglmath_mat_t m, size_t mr, size_t mc, size_t nr, size_t nc);

int moonglmath_testquat(lua_State *L, int arg, moonglmath_quat_t q);
int moonglmath_checkquat(lua_State *L, int arg, moonglmath_quat_t q);
int moonglmath_pushquat(lua_State *L, moonglmath_quat_t q);

int moonglmath_testcomplex(lua_State *L, int arg, moonglmath_complex_t *z);
int moonglmath_checkcomplex(lua_State *L, int arg, moonglmath_complex_t *z);
int moonglmath_pushcomplex(lua_State *L, moonglmath_complex_t z);

/*---------------------------------------------------------------------------*
 | Vector                                                                    |
 *---------------------------------------------------------------------------*/

#define moonglmath_vec_clear(v)    memset((v), 0, sizeof(moonglmath_vec_t))
#define moonglmath_vec_copy(dst,v) memcpy((dst), (v), sizeof(moonglmath_vec_t))
void moonglmath_vec_unm(moonglmath_vec_t dst, moonglmath_vec_t v, size_t n);
void moonglmath_vec_add(moonglmath_vec_t dst, moonglmath_vec_t v1, moonglmath_vec_t v2, size_t n);
void moonglmath_vec_sub(moonglmath_vec_t dst, moonglmath_vec_t v1, moonglmath_vec_t v2, size_t n);
double moonglmath_vec_norm(moonglmath_vec_t v, size_t n);
double moonglmath_vec_norm2(moonglmath_vec_t v, size_t n);
void moonglmath_vec_normalize(moonglmath_vec_t v, size_t n);
void moonglmath_vec_div(moonglmath_vec_t dst, moonglmath_vec_t v, double s, size_t n);
double moonglmath_vec_dot(moonglmath_vec_t v1, moonglmath_vec_t v2, size_t n);
void moonglmath_vec_vxs(moonglmath_vec_t dst, moonglmath_vec_t v, double s, size_t n);
void moonglmath_vec_vxv(moonglmath_mat_t dst, moonglmath_vec_t v1, moonglmath_vec_t v2, size_t n);
void moonglmath_vec_cross(moonglmath_vec_t dst, moonglmath_vec_t v1, moonglmath_vec_t v2);
void moonglmath_vec_clamp(moonglmath_vec_t dst, moonglmath_vec_t v, moonglmath_vec_t minv, moonglmath_vec_t maxv, size_t n);
void moonglmath_vec_mix(moonglmath_vec_t dst, moonglmath_vec_t v1, moonglmath_vec_t v2, size_t n, double k);
void moonglmath_vec_step(moonglmath_vec_t dst, moonglmath_vec_t v, moonglmath_vec_t edge, size_t n);
void moonglmath_vec_smoothstep(moonglmath_vec_t dst, moonglmath_vec_t v, moonglmath_vec_t edge0, moonglmath_vec_t edge1, size_t n);
void moonglmath_vec_fade(moonglmath_vec_t dst, moonglmath_vec_t v, moonglmath_vec_t edge0, moonglmath_vec_t edge1, size_t n);

/*---------------------------------------------------------------------------*
 | Box                                                                       |
 *---------------------------------------------------------------------------*/

#define moonglmath_box_clear(b)    memset((b), 0, sizeof(moonglmath_box_t))
#define moonglmath_box_copy(dst,b) memcpy((dst), (b), sizeof(moonglmath_box_t))

/*---------------------------------------------------------------------------*
 | Matrix                                                                    |
 *---------------------------------------------------------------------------*/

#define moonglmath_mat_clear(m)        memset((m), 0, sizeof(moonglmath_mat_t))
#define moonglmath_mat_copy(dst,m)     memcpy((dst), (m), sizeof(moonglmath_mat_t))
void moonglmath_mat_unm(moonglmath_mat_t dst, moonglmath_mat_t m, size_t nr, size_t nc);
void moonglmath_mat_transpose(moonglmath_mat_t dst, moonglmath_mat_t m, size_t nr, size_t nc);
void moonglmath_mat_add(moonglmath_mat_t dst, moonglmath_mat_t m1, moonglmath_mat_t m2, size_t nr, size_t nc);
void moonglmath_mat_sub(moonglmath_mat_t dst, moonglmath_mat_t m1, moonglmath_mat_t m2, size_t nr, size_t nc);
void moonglmath_mat_div(moonglmath_mat_t dst, moonglmath_mat_t m, double s, size_t nr, size_t nc);
void moonglmath_mat_mul(moonglmath_mat_t dst, moonglmath_mat_t m1, moonglmath_mat_t m2, size_t nr1, size_t nc1, size_t nc2);
void moonglmath_mat_mulby(moonglmath_mat_t m1, moonglmath_mat_t m2, size_t nr1, size_t nc1, size_t nc2);
void moonglmath_mat_mxs(moonglmath_mat_t dst, moonglmath_mat_t m, double s, size_t nr, size_t nc);
void moonglmath_mat_mxv(moonglmath_vec_t dst, moonglmath_mat_t m, moonglmath_vec_t v, size_t nr, size_t nc);
void moonglmath_mat_vxm(moonglmath_vec_t dst, moonglmath_vec_t v, moonglmath_mat_t m, size_t nr, size_t nc);
double moonglmath_mat_det2(moonglmath_mat_t m);
double moonglmath_mat_det3(moonglmath_mat_t m);
double moonglmath_mat_det4(moonglmath_mat_t m);
void moonglmath_mat_adj(moonglmath_mat_t dst, moonglmath_mat_t m, size_t n);
int moonglmath_mat_inv(moonglmath_mat_t dst, moonglmath_mat_t m, size_t n);
int moonglmath_mat_clamp(moonglmath_mat_t dst, moonglmath_mat_t m, moonglmath_mat_t minm, moonglmath_mat_t maxm, size_t nr, size_t nc);
int moonglmath_mat_mix(moonglmath_mat_t dst, moonglmath_mat_t m1, moonglmath_mat_t m2, size_t nr, size_t nc, double k);
int moonglmath_mat_step(moonglmath_mat_t dst, moonglmath_mat_t m, moonglmath_mat_t edge, size_t nr, size_t nc);
int moonglmath_mat_smoothstep(moonglmath_mat_t dst, moonglmath_mat_t m, moonglmath_mat_t edge0, moonglmath_mat_t edge1, size_t nr, size_t nc);
int moonglmath_mat_fade(moonglmath_mat_t dst, moonglmath_mat_t m, moonglmath_mat_t edge0, moonglmath_mat_t edge1, size_t nr, size_t nc);

/*---------------------------------------------------------------------------*
 | Quaternion                                                                |
 *---------------------------------------------------------------------------*/

#define moonglmath_quat_clear(q)   memset((q), 0, sizeof(moonglmath_quat_t))
#define moonglmath_quat_copy(dst, q)   memcpy((dst), (q), sizeof(moonglmath_quat_t))
void moonglmath_quat_unm(moonglmath_quat_t dst, moonglmath_quat_t q);
void moonglmath_quat_add(moonglmath_quat_t dst, moonglmath_quat_t q1, moonglmath_quat_t q2);
void moonglmath_quat_sub(moonglmath_quat_t dst, moonglmath_quat_t q1, moonglmath_quat_t q2);
double moonglmath_quat_norm(moonglmath_quat_t q);
double moonglmath_quat_norm2(moonglmath_quat_t q);
void moonglmath_quat_normalize(moonglmath_quat_t q);
void moonglmath_quat_conj(moonglmath_quat_t dst, moonglmath_quat_t q); 
void moonglmath_quat_inv(moonglmath_quat_t dst, moonglmath_quat_t q);
void moonglmath_quat_div(moonglmath_quat_t dst, moonglmath_quat_t q, double s);
void moonglmath_quat_mul(moonglmath_quat_t dst, moonglmath_quat_t q, moonglmath_quat_t p);
void moonglmath_quat_qxs(moonglmath_quat_t dst, moonglmath_quat_t q, double s);
void moonglmath_quat_mix(moonglmath_quat_t dst, moonglmath_quat_t q, moonglmath_quat_t p, double t);
void moonglmath_quat_slerp(moonglmath_quat_t dst, moonglmath_quat_t q, moonglmath_quat_t p, double t);

/*---------------------------------------------------------------------------*
 | Numbers                                                                   |
 *---------------------------------------------------------------------------*/

double moonglmath_clamp(double x, double minval, double maxval);
double moonglmath_mix(double x, double y, double k);
double moonglmath_step(double x, double edge);
double moonglmath_smoothstep(double x, double edge0, double edge1);
double moonglmath_fade(double x, double edge0, double edge1);

/*---------------------------------------------------------------------------*
 | Other                                                                     |
 *---------------------------------------------------------------------------*/

/* time */
double moonglmath_now(void);

/* basic transforms */
void moonglmath_translate(moonglmath_mat_t m, double x, double y, double z);
void moonglmath_scale(moonglmath_mat_t m, double x, double y, double z);
void moonglmath_rotate_x(moonglmath_mat_t m, double rad);
void moonglmath_rotate_y(moonglmath_mat_t m, double rad);
void moonglmath_rotate_z(moonglmath_mat_t m, double rad);

/* viewing */
int moonglmath_look_at(moonglmath_mat_t dst, moonglmath_vec_t eye, moonglmath_vec_t at, moonglmath_vec_t up);
int moonglmath_ortho(moonglmath_mat_t dst, double l, double r, double b, double t, double n, double f);
int moonglmath_frustum(moonglmath_mat_t dst, double l, double r, double b, double t, double n, double f);
int moonglmath_perspective(moonglmath_mat_t dst, double fovy, double aspect, double n, double f);

#endif /* moonglmathDEFINED */

