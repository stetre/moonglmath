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

int testvec(lua_State *L, int arg, vec_t v, size_t *size, unsigned int *isrow)
/* Tests if the element at arg is a vector and sets v, *size and *isrow accordingly
 * (unless they are passed as 0).
 */
    {
    size_t i;
    size_t size_;
    unsigned int isrow_;
    if(!testmetatable(L, arg, VEC_MT)) return 0;

    lua_getfield(L, arg, "type");   
    isrow_ = checkisrow(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, arg, "size");
    size_ = luaL_checkinteger(L, -1);
    lua_pop(L, 1);
    checkvecsize(L, size_);

    if(isrow != NULL) *isrow = isrow_;
    if(size != NULL) *size = size_;
    if(v != NULL)
        {
        vec_clear(v);
        for(i=0; i < size_; i++)
            {
            lua_geti(L, arg, i+1);
            v[i] = luaL_checknumber(L, -1);
            lua_pop(L, 1);
            }
        }
    return 1;
    }

int checkvec(lua_State *L, int arg, vec_t v, size_t *size, unsigned int *isrow)
/* Same as testvec(), but raises a error if the test fails.
 */
    {
    if(!testvec(L, arg, v, size, isrow))
        return luaL_argerror(L, arg, lua_pushfstring(L, "%s expected", VEC_MT));
    return 1;
    }


int pushvec(lua_State *L, vec_t v, size_t vsize, size_t size, unsigned int isrow)
/* vsize = no. of valid elements in v
 * if vsize > size, the exceeding elements are discarded,
 * if vsize < size, the missing elements are set to 0.
 */
    {
    size_t i;
    checkvecsize(L, size);
    lua_newtable(L);
    setmetatable(L, VEC_MT);
    for(i=0; i<size; i++)
        {
        lua_pushnumber(L, i < vsize ? v[i] : 0);
        lua_seti(L, -2, i+1);
        }
    lua_pushinteger(L, size);
    lua_setfield(L, -2, "size");
    pushisrow(L, isrow);
    lua_setfield(L, -2, "type");
    return 1;
    }

/*------------------------------------------------------------------------------*
 | Vec                                                                          |
 *------------------------------------------------------------------------------*/

static int Vec(lua_State *L, size_t size, unsigned int isrow)
/* creates a vector of size 'size', from a vector or a list of values 
 * the size is adapted by discarding exceeding values or adding zeros
 * in lieu of missing values.
 * vecN(v)
 * vecN([x1 ,[x2,...[,xN]]])
 */
    {
    vec_t v;
    size_t vsize, i, arg;
    arg = 1;
    if(testvec(L, arg, v, &vsize, NULL))
        return pushvec(L, v, vsize, size, isrow);
    checkvecsize(L, size);
    for(i = 0; i < size; i++)
        v[i] = luaL_optnumber(L, arg++, 0);
    return pushvec(L, v, size, size, isrow);
    }

static int Vec2(lua_State *L) { return Vec(L, 2, 0); }
static int Vec3(lua_State *L) { return Vec(L, 3, 0); }
static int Vec4(lua_State *L) { return Vec(L, 4, 0); }
static int Vec2r(lua_State *L) { return Vec(L, 2, 1); }
static int Vec3r(lua_State *L) { return Vec(L, 3, 1); }
static int Vec4r(lua_State *L) { return Vec(L, 4, 1); }

static int IsVec(lua_State *L, size_t size, unsigned int isrow)
    {
    size_t vsize;
    unsigned int visrow;
    if(!testvec(L, 1, NULL, &vsize, &visrow))
        lua_pushboolean(L, 0);
    else
        lua_pushboolean(L, (vsize==size) && (visrow==isrow));
    return 1;
    }
    
static int IsVec2(lua_State *L) { return IsVec(L, 2, 0); }
static int IsVec3(lua_State *L) { return IsVec(L, 3, 0); }
static int IsVec4(lua_State *L) { return IsVec(L, 4, 0); }
static int IsVec2r(lua_State *L) { return IsVec(L, 2, 1); }
static int IsVec3r(lua_State *L) { return IsVec(L, 3, 1); }
static int IsVec4r(lua_State *L) { return IsVec(L, 4, 1); }

/*------------------------------------------------------------------------------*
 | ToString                                                                     |
 *------------------------------------------------------------------------------*/

#if 0
static int ToString(lua_State *L)
    {
    vec_t v;
    size_t size;
    unsigned int isrow;
    checkvec(L, 1, v, &size, &isrow);
    switch(size)
        {
#define T isrow==1 ? "" : "'"
        case 2: lua_pushfstring(L, "[%f %f]%s", v[0], v[1], T ); break;
        case 3: lua_pushfstring(L, "[%f %f %f]%s", v[0], v[1], v[2], T); break;
        case 4: lua_pushfstring(L, "[%f %f %f %f]%s", v[0], v[1], v[2], v[3], T); break;
        default:
            return luaL_error(L, UNEXPECTED_ERROR);
        }
    return 1;
    }
#endif

static int ToString_(lua_State *L, vec_t v, size_t size, unsigned int isrow)
/* With snprintf because lua_pushfstring does not support the %g conversion specifier */
    {
#define SZ 512
#define space (SZ-p)
    char str[SZ];
    int p=0;
    switch(size)
        {
#define T isrow==1 ? "" : "'"
        case 2: p = snprintf(str, space, "{ "FMT", "FMT" }%s", v[0], v[1], T ); break;
        case 3: p = snprintf(str, space, "{ "FMT", "FMT", "FMT" }%s", 
                                                v[0], v[1], v[2], T); break;
        case 4: p = snprintf(str, space, "{ "FMT", "FMT", "FMT", "FMT" }%s", 
                                                v[0], v[1], v[2], v[3], T); break;
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
    vec_t v;
    size_t size;
    unsigned int isrow;
    checkvec(L, 1, v, &size, &isrow);
    return ToString_(L, v, size, isrow);
    }


static int Concat(lua_State *L)
    {
    vec_t v;
    size_t size;
    unsigned int isrow;
    if(testvec(L, 1, v, &size, &isrow))
        {
        ToString_(L, v, size, isrow);
        lua_pushvalue(L, 2);
        }
    else if(testvec(L, 2, v, &size, &isrow))
        {
        lua_pushvalue(L, 1);
        ToString_(L, v, size, isrow);
        }
    else 
        return unexpected(L);
    lua_concat(L, 2);
    return 1;
    }


/*------------------------------------------------------------------------------*
 | Non-Lua functions (for internal use)                                         |
 *------------------------------------------------------------------------------*/

void vec_unm(vec_t dst, vec_t v, size_t n)
    {
    size_t i;
    vec_clear(dst);
    for(i=0; i < n; i++)    
        dst[i] = -v[i];
    }
    
void vec_add(vec_t dst, vec_t v1, vec_t v2, size_t n)
    {
    size_t i;
    vec_clear(dst);
    for(i=0; i < n; i++)    
        dst[i] = v1[i] + v2[i];
    }
    
void vec_sub(vec_t dst, vec_t v1, vec_t v2, size_t n)
    {
    size_t i;
    vec_clear(dst);
    for(i=0; i < n; i++)    
        dst[i] = v1[i] - v2[i];
    }
    
double vec_norm(vec_t v, size_t n)
    {
    size_t i;
    double r = 0;
    for(i=0; i < n; i++)
        r += v[i]*v[i];
    return sqrt(r);
    }

double vec_norm2(vec_t v, size_t n)
    {
    size_t i;
    double r = 0;
    for(i=0; i < n; i++)
        r += v[i]*v[i];
    return r;
    }

void vec_normalize(vec_t v, size_t n) 
/* in place */
    {
    size_t i;
    double norm = vec_norm(v, n);
    for(i=0; i < n; i++)    
        v[i] = v[i]/norm;
    }
    
void vec_div(vec_t dst, vec_t v, double s, size_t n)
    {
    size_t i; 
    vec_clear(dst);
    for(i=0; i < n; i++)    
        dst[i] = v[i] / s;
    }

double vec_dot(vec_t v1, vec_t v2, size_t n)
/* dot product */
    {
    size_t i;
    double s = 0;
    for(i=0; i < n; i++)    
        s += (v1[i]*v2[i]);
    return s;
    }

void vec_vxs(vec_t dst, vec_t v, double s, size_t n)
    {
    size_t i; 
    for(i=0; i < n; i++)    
        dst[i] = v[i] * s;
    }

void vec_vxv(mat_t dst, vec_t v1, vec_t v2, size_t n)
/* v1 is a nx1 column vector
 * v2 is a 1xn row vector
 * m is a nxn square matrix (n x 1) * (1 x n) = (n x n)
 */
    {
    size_t i, j;
    for(i=0; i < n; i++)
        for(j=0; j < n; j++)
            dst[i][j] = v1[i] * v2[j];
    }

void vec_cross(vec_t dst, vec_t v1, vec_t v2)
/* dst is always to be regarded as a size=3 vector */
    {
    dst[0] = v1[1]*v2[2] - v1[2]*v2[1];
    dst[1] = v1[2]*v2[0] - v1[0]*v2[2];
    dst[2] = v1[0]*v2[1] - v1[1]*v2[0];
    dst[3] = 0;
    }

void vec_clamp(vec_t dst, vec_t v, vec_t minv, vec_t maxv, size_t n)
    {
    size_t i;
    for(i=0; i<n; i++)
        dst[i] = clamp(v[i], minv[i], maxv[i]);
    }

void vec_mix(vec_t dst, vec_t v1, vec_t v2, size_t n, double k)
    {
    size_t i;
    for(i=0; i<n; i++)
        dst[i] = mix(v1[i], v2[i], k);
    }

void vec_step(vec_t dst, vec_t v, vec_t edge, size_t n)
    {
    size_t i;
    for(i=0; i<n; i++)
        dst[i] = step(v[i], edge[i]);
    }

void vec_smoothstep(vec_t dst, vec_t v, vec_t edge0, vec_t edge1, size_t n)
    {
    size_t i;
    for(i=0; i<n; i++)
        dst[i] = smoothstep(v[i], edge0[i], edge1[i]);
    }

void vec_fade(vec_t dst, vec_t v, vec_t edge0, vec_t edge1, size_t n)
    {
    size_t i;
    for(i=0; i<n; i++)
        dst[i] = fade(v[i], edge0[i], edge1[i]);
    }


/*------------------------------------------------------------------------------*
 | Metamethods                                                                  |
 *------------------------------------------------------------------------------*/

static int Unm(lua_State *L)
    {
    vec_t v; 
    size_t size, i; 
    unsigned int isrow;
    checkvec(L, 1, v, &size, &isrow);
    for(i=0; i < 4; i++)    
        v[i] = -v[i];
    return pushvec(L, v, size, size, isrow);
    }


static int Add(lua_State *L)
    {
    vec_t v, v1; 
    size_t size, size1, i; 
    unsigned int isrow, isrow1;
    checkvec(L, 1, v, &size, &isrow);
    checkvec(L, 2, v1, &size1, &isrow1);
    if((isrow != isrow1) || (size != size1))
        return luaL_error(L, OPERANDS_ERROR);
    for(i=0; i < size; i++) 
        v[i] += v1[i];
    return pushvec(L, v, size, size, isrow);
    }

static int Sub(lua_State *L)
    {
    vec_t v, v1; 
    size_t size, size1, i; 
    unsigned int isrow, isrow1;
    checkvec(L, 1, v, &size, &isrow);
    checkvec(L, 2, v1, &size1, &isrow1);
    if((isrow != isrow1) || (size != size1))
        return luaL_error(L, OPERANDS_ERROR);
    for(i=0; i < size; i++) 
        v[i] -= v1[i];
    return pushvec(L, v, size, size, isrow);
    }

static int Vxs(lua_State *L, int sarg, int varg)
    {
    vec_t v; 
    size_t size, i; 
    unsigned int isrow;
    double s = luaL_checknumber(L, sarg);
    checkvec(L, varg, v, &size, &isrow);
    for(i=0; i < size; i++) 
        v[i] *= s;
    return pushvec(L, v, size, size, isrow);
    }

static int Vxv(lua_State *L, vec_t v1, size_t nr,  vec_t v2, size_t nc) 
/* Nx1 * 1xM = NxM   (column) x (row) = matrix */
    {
    mat_t m;
    size_t i, j;
    for(i=0; i < nr; i++)
        for(j=0; j < nc; j++)
            m[i][j] = v1[i] * v2[j];
    return pushmat(L, m, nr, nc, nr, nc);
    }

static int Vxm(lua_State *L) /* 1xM * M*N = 1*N (row vector)*/
    {
    unsigned int isrow;
    size_t i, j, nr, nc, size;
    mat_t m;
    vec_t v, v1;
    double s;
    checkvec(L, 1, v, &size, &isrow);
    checkmat(L, 2, m, &nr, &nc);
    if( (!isrow) || (size != nr))
        return luaL_error(L, OPERANDS_ERROR);
    for(i=0; i < nc; i++)
        {
        s = 0;
        for(j=0; j < size; j++)
            s += (v[j] * m[j][i]);
        v1[i] = s;
        }
    return pushvec(L, v1, nc, nc, 1);
    }

static int Mul(lua_State *L)
    {
    vec_t v, v1; 
    size_t size, size1, i; 
    unsigned int isrow, isrow1;
    double r = 0;
    if(lua_isnumber(L, 1))
        return Vxs(L, 1, 2);
    if(lua_isnumber(L, 2))
        return Vxs(L, 2, 1);
    if(testmat(L, 2, NULL, NULL, NULL))
        return Vxm(L);
    checkvec(L, 1, v, &size, &isrow);
    checkvec(L, 2, v1, &size1, &isrow1);
    if((!isrow) && isrow1) /* Nx1 * 1xM = NxM */
        return Vxv(L, v, size, v1, size1);
    if(size != size1)
        return luaL_error(L, OPERANDS_ERROR);
    /* dot product:  1*N * Nx1, but also Nx1 * Nx1 and 1xN * 1*N are admitted */
    for(i=0; i < size; i++) 
        r += (v[i]*v1[i]);
    lua_pushnumber(L, r);
    return 1;
    }

static int Div(lua_State *L)
    {
    vec_t v; 
    size_t size, i; 
    unsigned int isrow;
    double s;
    checkvec(L, 1, v, &size, &isrow);
    s = luaL_checknumber(L, 2);
    for(i=0; i < size; i++) 
        v[i] /= s;
    return pushvec(L, v, size, size, isrow);
    }

static int Cross(lua_State *L)
/* always return a size=3 vector */
//@@ TODO?: v % scalar = component-wise mod
    {
    vec_t v1, v2, v;
    size_t size1, size2; 
    unsigned int isrow1, isrow2;
    checkvec(L, 1, v1, &size1, &isrow1);
    checkvec(L, 2, v2, &size2, &isrow2);
    if((isrow1 != isrow2) || (size1 != size2))
        return luaL_error(L, OPERANDS_ERROR);
    if(size1==2)
        v1[2] = v2[2] = 0;
    vec_cross(v, v1, v2);
    return pushvec(L, v, 3, 3, isrow1);
    }

int vec_Norm(lua_State *L)
    {
    vec_t v;
    size_t size, i;
    double r = 0;
    checkvec(L, 1, v, &size, NULL);
    for(i=0; i < size; i++) 
        r += v[i]*v[i];
    lua_pushnumber(L, sqrt(r));
    return 1;
    }

int vec_Norm2(lua_State *L)
    {
    vec_t v;
    size_t size, i;
    double r = 0;
    checkvec(L, 1, v, &size, NULL);
    for(i=0; i < size; i++)
        r += v[i]*v[i];
    lua_pushnumber(L, r);
    return 1;
    }

int vec_Normalize(lua_State *L)
    {
    vec_t v; 
    size_t size; 
    unsigned int isrow;
    checkvec(L, 1, v, &size, &isrow);
    vec_normalize(v, size);
    return pushvec(L, v, size, size, isrow);
    }


int vec_Transpose(lua_State *L)
    {
    vec_t v; size_t size; unsigned int isrow;
    checkvec(L, 1, v, &size, &isrow);
    return pushvec(L, v, size, size, !isrow);
    }

int vec_Clamp(lua_State *L)
    {
    vec_t dst, v, minv, maxv;
    size_t size, size1;
    unsigned int isrow, isrow1;
    checkvec(L, 1, v, &size, &isrow);
    checkvec(L, 2, minv, &size1, &isrow1);
    if(size1 != size || isrow1 != isrow)
        return luaL_error(L, OPERANDS_ERROR);
    checkvec(L, 3, maxv, &size1, &isrow1);
    if(size1 != size || isrow1 != isrow)
        return luaL_error(L, OPERANDS_ERROR);
    vec_clamp(dst, v, minv, maxv, size);
    return pushvec(L, dst, size, size, isrow);
    }

int vec_Mix(lua_State *L)
    {
    vec_t dst, v1, v2;
    double k;
    size_t size, size1;
    unsigned int isrow, isrow1;
    checkvec(L, 1, v1, &size, &isrow);
    checkvec(L, 2, v2, &size1, &isrow1);
    if(size1 != size || isrow1 != isrow)
        return luaL_error(L, OPERANDS_ERROR);
    k = luaL_checknumber(L, 3);
    vec_mix(dst, v1, v2, size, k);
    return pushvec(L, dst, size, size, isrow);
    }

int vec_Step(lua_State *L)
    {
    vec_t dst, v1, edge;
    double k;
    size_t size, size1;
    unsigned int isrow, isrow1;
    checkvec(L, 1, v1, &size, &isrow);
    if(lua_isnumber(L, 2))
        {
        k = lua_tonumber(L, 2);
        edge[0]=edge[1]=edge[2]=edge[3]=k;
        }
    else
        {
        checkvec(L, 2, edge, &size1, &isrow1);
        if(size1 != size || isrow1 != isrow)
            return luaL_error(L, OPERANDS_ERROR);
        }
    vec_step(dst, v1, edge, size);
    return pushvec(L, dst, size, size, isrow);
    }


int vec_Smoothstep(lua_State *L)
    {
    vec_t dst, v, edge0, edge1;
    double k;
    size_t size, size1;
    unsigned int isrow, isrow1;
    checkvec(L, 1, v, &size, &isrow);
    if(lua_isnumber(L, 2))
        {
        k = lua_tonumber(L, 2);
        edge0[0]=edge0[1]=edge0[2]=edge0[3]=k;
        }
    else
        {
        checkvec(L, 2, edge0, &size1, &isrow1);
        if(size1 != size || isrow1 != isrow)
            return luaL_error(L, OPERANDS_ERROR);
        }
    if(lua_isnumber(L, 3))
        {
        k = lua_tonumber(L, 3);
        edge1[0]=edge1[1]=edge1[2]=edge1[3]=k;
        }
    else
        {
        checkvec(L, 3, edge1, &size1, &isrow1);
        if(size1 != size || isrow1 != isrow)
            return luaL_error(L, OPERANDS_ERROR);
        }
    vec_smoothstep(dst, v, edge0, edge1, size);
    return pushvec(L, dst, size, size, isrow);
    }

int vec_Fade(lua_State *L)
    {
    vec_t dst, v, edge0, edge1;
    double k;
    size_t size, size1;
    unsigned int isrow, isrow1;
    checkvec(L, 1, v, &size, &isrow);
    if(lua_isnumber(L, 2))
        {
        k = lua_tonumber(L, 2);
        edge0[0]=edge0[1]=edge0[2]=edge0[3]=k;
        }
    else
        {
        checkvec(L, 2, edge0, &size1, &isrow1);
        if(size1 != size || isrow1 != isrow)
            return luaL_error(L, OPERANDS_ERROR);
        }
    if(lua_isnumber(L, 3))
        {
        k = lua_tonumber(L, 3);
        edge1[0]=edge1[1]=edge1[2]=edge1[3]=k;
        }
    else
        {
        checkvec(L, 3, edge1, &size1, &isrow1);
        if(size1 != size || isrow1 != isrow)
            return luaL_error(L, OPERANDS_ERROR);
        }
    vec_fade(dst, v, edge0, edge1, size);
    return pushvec(L, dst, size, size, isrow);
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
        { "__mod", Cross },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Methods[] = 
    {
        { "norm", vec_Norm },
        { "norm2", vec_Norm2 },
        { "normalize", vec_Normalize },
        { "transpose", vec_Transpose },
        { "clamp", vec_Clamp },
        { "mix", vec_Mix },
        { "step", vec_Step },
        { "smoothstep", vec_Smoothstep },
        { "fade", vec_Fade },
        { NULL, NULL } /* sentinel */
    };


/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "vec2", Vec2 },
        { "vec3", Vec3 },
        { "vec4", Vec4 },
        { "vec2r", Vec2r },
        { "vec3r", Vec3r },
        { "vec4r", Vec4r },
        { "isvec2", IsVec2 },
        { "isvec3", IsVec3 },
        { "isvec4", IsVec4 },
        { "isvec2r", IsVec2r },
        { "isvec3r", IsVec3r },
        { "isvec4r", IsVec4r },
        { NULL, NULL } /* sentinel */
    };

void moonglmath_open_vec(lua_State *L)
    {
    newmetatable(L, VEC_MT);
    metatable_setfuncs(L, VEC_MT, Metamethods, Methods);
    luaL_setfuncs(L, Functions, 0);
    }

