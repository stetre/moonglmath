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
 * MoonGLMATH library - C API redefinitions                                 *
 ****************************************************************************
 *
 * Redefines the C API functions and types without the 'moonglmath_' prefix.
 */

#ifndef moonglmath_localDEFINED
#define moonglmath_localDEFINED

#include "moonglmath.h"

#define vec_t moonglmath_vec_t
#define mat_t moonglmath_mat_t
#define quat_t moonglmath_quat_t

#define VEC_MT MOONGLMATH_VEC_MT
#define MAT_MT MOONGLMATH_MAT_MT
#define QUAT_MT MOONGLMATH_QUAT_MT

#define testmetatable moonglmath_testmetatable
#define checkmetatable moonglmath_checkmetatable

#define isvec moonglmath_isvec
#define testvec moonglmath_testvec
#define checkvec moonglmath_checkvec
#define pushvec moonglmath_pushvec

#define ismat moonglmath_ismat
#define testmat moonglmath_testmat
#define checkmat moonglmath_checkmat
#define pushmat moonglmath_pushmat

#define isquat moonglmath_isquat
#define testquat moonglmath_testquat
#define checkquat moonglmath_checkquat
#define pushquat moonglmath_pushquat

#define vec_clear moonglmath_vec_clear
#define vec_copy moonglmath_vec_copy
#define vec_unm moonglmath_vec_unm
#define vec_add moonglmath_vec_add
#define vec_sub moonglmath_vec_sub
#define vec_norm moonglmath_vec_norm
#define vec_normalize moonglmath_vec_normalize
#define vec_div moonglmath_vec_div
#define vec_dot moonglmath_vec_dot
#define vec_vxs moonglmath_vec_vxs
#define vec_vxv moonglmath_vec_vxv
#define vec_cross moonglmath_vec_cross

#define mat_clear moonglmath_mat_clear
#define mat_copy moonglmath_mat_copy
#define mat_unm moonglmath_mat_unm
#define mat_transpose moonglmath_mat_transpose
#define mat_add moonglmath_mat_add
#define mat_sub moonglmath_mat_sub
#define mat_div moonglmath_mat_div
#define mat_mul moonglmath_mat_mul
#define mat_mulby moonglmath_mat_mulby
#define mat_mxs moonglmath_mat_mxs
#define mat_mxv moonglmath_mat_mxv
#define mat_vxm moonglmath_mat_vxm
#define mat_det2 moonglmath_mat_det2
#define mat_det3 moonglmath_mat_det3
#define mat_det4 moonglmath_mat_det4
#define mat_adj moonglmath_mat_adj
#define mat_inv moonglmath_mat_inv

#define quat_clear moonglmath_quat_clear
#define quat_copy moonglmath_quat_copy
#define quat_unm moonglmath_quat_unm
#define quat_add moonglmath_quat_add
#define quat_sub moonglmath_quat_sub
#define quat_norm moonglmath_quat_norm
#define quat_normalize moonglmath_quat_normalize
#define quat_conj moonglmath_quat_conj
#define quat_inv moonglmath_quat_inv
#define quat_div moonglmath_quat_div
#define quat_mul moonglmath_quat_mul
#define quat_qxs moonglmath_quat_qxs

#define now moonglmath_now

#define translate moonglmath_translate
#define scale moonglmath_scale
#define rotate_x moonglmath_rotate_x
#define rotate_y moonglmath_rotate_y
#define rotate_z moonglmath_rotate_z


#define look_at moonglmath_look_at
#define ortho moonglmath_ortho
#define frustum moonglmath_frustum
#define perspective moonglmath_perspective

#endif /* moonglmath_localDEFINED */
