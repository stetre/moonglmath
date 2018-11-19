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

#ifndef enumsDEFINED
#define enumsDEFINED

/* enums.c */
#define enums_free_all moonglmath_enums_free_all
void enums_free_all(lua_State *L);
#define enums_test moonglmath_enums_test
uint32_t enums_test(lua_State *L, uint32_t domain, int arg, int *err);
#define enums_check moonglmath_enums_check
uint32_t enums_check(lua_State *L, uint32_t domain, int arg);
#define enums_push moonglmath_enums_push
int enums_push(lua_State *L, uint32_t domain, uint32_t code);
#define enums_values moonglmath_enums_values
int enums_values(lua_State *L, uint32_t domain);
#define enums_checklist moonglmath_enums_checklist
uint32_t* enums_checklist(lua_State *L, uint32_t domain, int arg, uint32_t *count, int *err);
#define enums_freelist moonglmath_enums_freelist
void enums_freelist(lua_State *L, uint32_t *list);


/* Enum domains */
#define DOMAIN_ISROW                      0
#define DOMAIN_TYPE                       1

/* DOMAIN_ISROW values (vector type) */
#define MOONGLMATH_COLUMN   0
#define MOONGLMATH_ROW      1

/* Types for glmath.sizeof() & friends */
#define MOONGLMATH_TYPE_CHAR         1
#define MOONGLMATH_TYPE_UCHAR        2
#define MOONGLMATH_TYPE_SHORT        3
#define MOONGLMATH_TYPE_USHORT       4
#define MOONGLMATH_TYPE_INT          5
#define MOONGLMATH_TYPE_UINT         6
#define MOONGLMATH_TYPE_LONG         7
#define MOONGLMATH_TYPE_ULONG        8
#define MOONGLMATH_TYPE_FLOAT        9
#define MOONGLMATH_TYPE_DOUBLE       10

#define testisrow(L, arg, err) (uint32_t)enums_test((L), DOMAIN_ISROW, (arg), (err))
#define checkisrow(L, arg) (uint32_t)enums_check((L), DOMAIN_ISROW, (arg))
#define pushisrow(L, val) enums_push((L), DOMAIN_ISROW, (uint32_t)(val))
#define valuesisrow(L) enums_values((L), DOMAIN_ISROW)

#define testtype(L, arg, err) (uint32_t)enums_test((L), DOMAIN_TYPE, (arg), (err))
#define checktype(L, arg) (uint32_t)enums_check((L), DOMAIN_TYPE, (arg))
#define pushtype(L, val) enums_push((L), DOMAIN_TYPE, (uint32_t)(val))
#define valuestype(L) enums_values((L), DOMAIN_TYPE)

#if 0 /* scaffolding 6yy */
#define testxxx(L, arg, err) (uint32_t)enums_test((L), DOMAIN_XXX, (arg), (err))
#define checkxxx(L, arg) (uint32_t)enums_check((L), DOMAIN_XXX, (arg))
#define pushxxx(L, val) enums_push((L), DOMAIN_XXX, (uint32_t)(val))
#define valuesxxx(L) enums_values((L), DOMAIN_XXX)
    CASE(xxx);

#endif

#endif /* enumsDEFINED */


