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

int testmat(lua_State *L, int arg, mat_t m, size_t *nr, size_t *nc)
    {
    int row;
    size_t i, j, nr_, nc_;
    if(!testmetatable(L, arg, MAT_MT))
        return 0;

    lua_getfield(L, arg, "rows");   
    nr_ = luaL_checkinteger(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, arg, "columns");    
    nc_ = luaL_checkinteger(L, -1);
    lua_pop(L, 1);
    checkmatsize(L, nr_, nc_);

    if(nr) *nr = nr_;
    if(nc) *nc = nc_;
    if(m)
        {
        for(i=0; i < nr_; i++)
            {
            if(lua_geti(L, arg, i+1) != LUA_TTABLE)
                return luaL_error(L, "malformed matrix");
            row = lua_gettop(L);
            for(j=0; j < nc_; j++)
                {
                lua_geti(L, row, j+1);
                m[i][j] = luaL_checknumber(L, -1);
                lua_pop(L, 1);
                }
            lua_pop(L, 1);
            }
        }
    return 1;
    }

int checkmat(lua_State *L, int arg, mat_t m, size_t *nr, size_t *nc)
/* Same as testmat(), but raises a error if the test fails.
 */
    {
    if(!testmat(L, arg, m, nr, nc))
        return luaL_argerror(L, arg, lua_pushfstring(L, "%s expected", MAT_MT));
    return 1;
    }

int pushmat(lua_State *L, mat_t m, size_t mr, size_t mc, size_t nr, size_t nc)
/* mr, mc = no. of valid colums/rows in m
 */
    {
    size_t i, j;
    checkmatsize(L, nr, nc);
    lua_newtable(L);
    setmetatable(L, MAT_MT);
    for(i=0; i<nr; i++)
        {
        lua_newtable(L); /* row i+1 */
        for(j=0; j<nc; j++)
            {
            lua_pushnumber(L, (i < mr) && (j < mc) ? m[i][j] : 0);
            lua_seti(L, -2, j+1);
            }
        lua_seti(L, -2, i+1);
        }
    for(i=nr; i<4; i++)
        {
        lua_newtable(L); /* empty rows */
        lua_seti(L, -2, i+1);
        }
    lua_pushinteger(L, nr);
    lua_setfield(L, -2, "rows");
    lua_pushinteger(L, nc);
    lua_setfield(L, -2, "columns");
    return 1;
    }

/*------------------------------------------------------------------------------*
 | Mat                                                                          |
 *------------------------------------------------------------------------------*/

static int Matfromvecs(lua_State *L, size_t nr, size_t nc, unsigned int row)
/* if row=1, expects all row vectors
 * if row=0, expects all column vectors
 */
    {
    size_t i,j, n;  
    mat_t m;
    vec_t v[4];
    size_t size[4];
    unsigned int isrow[4];
    
    for(i=0; i<4; i++)
        vec_clear(v[i]);
    
    n = row==1 ? nr : nc;
    for(i=0; i < n; i++)
        {
        if(testvec(L, i+1, v[i], &size[i], &isrow[i]) && (isrow[i] != row))
            return luaL_argerror(L, i+i, "expected all row or all column vectors");
        }

    if(row)
        {
        /* v[i] = i-th row */
        for(i=0; i<nr; i++)
            for(j=0; j<nc; j++)
                m[i][j] = v[i][j];
        }
    else
        {
        /* v[j] = j-th column */
        for(i=0; i<nr; i++)
            for(j=0; j<nc; j++)
                m[i][j] = v[j][i];
        }
    return pushmat(L, m, nr, nc, nr, nc);
    }


static int Mat(lua_State *L, size_t nr, size_t nc)
/* mat()  -> square identity
 * mat(m)  m=matrix (any size)
 * mat(v1,...,vnr) nr row vectors,
 * mat(v1,...,vnc) nc column vectors,
 * mat(val1, val2, ...)  values in row-major order
 */
    {
    mat_t m;
    size_t mr, mc, i, j, arg;
    unsigned int isrow;
    checkmatsize(L, nr, nc);
    arg = 1;
    if(lua_isnoneornil(L, arg)) /* square identity (or all zeros if nr != nc) */
        {
        mat_clear(m);
        if(nr == nc)
            for(i = 0; i< nr; i++) m[i][i] = 1;
        return pushmat(L, m, nr, nc, nr, nc);
        }

    if(testmat(L, arg, m, &mr, &mc)) /* initialization from matrix */
        return pushmat(L, m, mr, mc, nr, nc);

    if(testvec(L, arg, NULL, NULL, &isrow)) /*initialization from vectors */
        return Matfromvecs(L, nr, nc, isrow);

    checkmatsize(L, nr, nc);
    for(i = 0; i < nr; i++)
        for(j = 0; j < nc; j++)
            m[i][j] = luaL_optnumber(L, arg++, 0);
    return pushmat(L, m, nr, nc, nr, nc);
    }

static int Mat2(lua_State *L) { return Mat(L, 2, 2); }
static int Mat3(lua_State *L) { return Mat(L, 3, 3); }
static int Mat4(lua_State *L) { return Mat(L, 4, 4); }
static int Mat2x3(lua_State *L) { return Mat(L, 2, 3); }
static int Mat3x2(lua_State *L) { return Mat(L, 3, 2); }
static int Mat2x4(lua_State *L) { return Mat(L, 2, 4); }
static int Mat4x2(lua_State *L) { return Mat(L, 4, 2); }
static int Mat3x4(lua_State *L) { return Mat(L, 3, 4); }
static int Mat4x3(lua_State *L) { return Mat(L, 4, 3); }

static int IsMat(lua_State *L, size_t nr, size_t nc)
    {
    size_t nr_, nc_;
    if(!testmat(L, 1, NULL, &nr_, &nc_))
        lua_pushboolean(L, 0);
    else
        lua_pushboolean(L, (nr_==nr) && (nc_==nc));
    return 1;
    }
    
static int IsMat2(lua_State *L) { return IsMat(L, 2, 2); }
static int IsMat3(lua_State *L) { return IsMat(L, 3, 3); }
static int IsMat4(lua_State *L) { return IsMat(L, 4, 4); }
static int IsMat2x3(lua_State *L) { return IsMat(L, 2, 3); }
static int IsMat3x2(lua_State *L) { return IsMat(L, 3, 2); }
static int IsMat2x4(lua_State *L) { return IsMat(L, 2, 4); }
static int IsMat4x2(lua_State *L) { return IsMat(L, 4, 2); }
static int IsMat3x4(lua_State *L) { return IsMat(L, 3, 4); }
static int IsMat4x3(lua_State *L) { return IsMat(L, 4, 3); }


/*------------------------------------------------------------------------------*
 | ToString                                                                     |
 *------------------------------------------------------------------------------*/

static int ToString_(lua_State *L, mat_t m, size_t nr, size_t nc)
/* With snprintf because lua_pushfstring does not support the %g conversion specifier */
    {
#define SZ 512
#define space (SZ-p)
    char str[SZ];
    int p=0;
    size_t i;
    p += snprintf(str, space, "{");
    for(i=0; i < nr; i++)
        {
        switch(nc)
            {
            case 2: p+=snprintf(str+p, space, "{ "FMT", "FMT" }, ", 
                            m[i][0], m[i][1]); break;
            case 3: p+=snprintf(str+p, space, "{ "FMT", "FMT", "FMT" }, ",
                            m[i][0], m[i][1], m[i][2]); break;
            case 4: p+=snprintf(str+p, space, "{ "FMT", "FMT", "FMT", "FMT" }, ", 
                            m[i][0], m[i][1], m[i][2], m[i][3]); break;
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
        }
    p-=2; /* strip the last separator */
    p+=snprintf(str+p, space, "}");
    lua_pushlstring(L, str, p);
    return 1;
#undef space
#undef SZ
    }

static int ToString(lua_State *L)
    {
    size_t nr, nc;
    mat_t m;
    checkmat(L, 1, m, &nr, &nc);
    return ToString_(L, m, nr, nc);
    }

static int Concat(lua_State *L)
    {
    size_t nr, nc;
    mat_t m;
//  printf("%s %s\n", lua_typename(L, lua_type(L, 1)), lua_typename(L, lua_type(L, 2)));
    if(testmat(L, 1, m, &nr, &nc))
        {
        ToString_(L, m, nr, nc);
        lua_pushvalue(L, 2);
        }
    else if(testmat(L, 2, m, &nr, &nc))
        {
        lua_pushvalue(L, 1);
        ToString_(L, m, nr, nc);
        }
    else 
        return unexpected(L);
    lua_concat(L, 2);
    return 1;
    }


/*------------------------------------------------------------------------------*
 | Non-Lua Functions (for internal use)                                         |
 *------------------------------------------------------------------------------*/

/* determinant */
#define det2_(a,b,c,d) ((a)*(d) - (c)*(b))
#define det3_(a,b,c,d,e,f,g,h,i)    \
    ((a)*det2_((e),(f),(h),(i))     \
    -(b)*det2_((d),(f),(g),(i))     \
    + (c)*det2_((d),(e),(g),(h)))
#define det4_(a,b,c,d,e,f,g,h,i,l,m,n,o,p,q,r)      \
    ((a)*det3_((f),(g),(h),(l),(m),(n),(p),(q),(r)) \
    -(b)*det3_((e),(g),(h),(i),(m),(n),(o),(q),(r)) \
    +(c)*det3_((e),(f),(h),(i),(l),(n),(o),(p),(r)) \
    -(d)*det3_((e),(f),(g),(i),(l),(m),(o),(p),(q)))

#define det2(M) det2_((M)[0][0],(M)[0][1],(M)[1][0],(M)[1][1])
#define det3(M) det3_((M)[0][0],(M)[0][1],(M)[0][2],    \
                      (M)[1][0],(M)[1][1],(M)[1][2],    \
                      (M)[2][0],(M)[2][1],(M)[2][2])
#define det4(M) det4_((M)[0][0],(M)[0][1],(M)[0][2],(M)[0][3],  \
                      (M)[1][0],(M)[1][1],(M)[1][2],(M)[1][3],  \
                      (M)[2][0],(M)[2][1],(M)[2][2],(M)[2][3],  \
                      (M)[3][0],(M)[3][1],(M)[3][2],(M)[3][3])

double mat_det2(mat_t m)
    { return det2(m); }

double mat_det3(mat_t m)
    { return det3(m); }

double mat_det4(mat_t m)
    { return det4(m); }

void mat_unm(mat_t dst, mat_t m, size_t nr, size_t nc)
    {
    size_t i, j;
    for(i = 0; i < nr; i++)
        for(j = 0; j < nc; j++)
            dst[i][j] = -m[i][j];
    }

void mat_transpose(mat_t dst, mat_t m, size_t nr, size_t nc)
    {
    size_t i, j;
    for(i = 0; i < nr; i++)
        for(j = 0; j < nc; j++)
            dst[i][j] = m[j][i];
    }

void mat_add(mat_t dst, mat_t m1, mat_t m2, size_t nr, size_t nc)
    {
    size_t i, j;
    for(i = 0; i < nr; i++)
        for(j = 0; j < nc; j++)
            dst[i][j] = m1[i][j] + m2[i][j];
    }

void mat_sub(mat_t dst, mat_t m1, mat_t m2, size_t nr, size_t nc)
    {
    size_t i, j;
    for(i = 0; i < nr; i++)
        for(j = 0; j < nc; j++)
            dst[i][j] = m1[i][j] - m2[i][j];
    }

void mat_div(mat_t dst, mat_t m, double s, size_t nr, size_t nc)
    {
    size_t i, j;
    for(i = 0; i < nr; i++)
        for(j = 0; j < nc; j++)
            dst[i][j] = m[i][j]/s;
    }

void mat_mxs(mat_t dst, mat_t m, double s, size_t nr, size_t nc)
    {
    size_t i, j;
    for(i = 0; i < nr; i++)
        for(j = 0; j < nc; j++)
            dst[i][j] = m[i][j]*s;
    }


void mat_mul(mat_t dst, mat_t m1, mat_t m2, size_t nr1, size_t nc1, size_t nc2) /* nr1=nc1 */
/* dst = m1 * m2  
 * (nr1 x nc2) = (nr1 x nc1) *(nc1 x nc2)
 */
    {
    size_t i, j, k;
    double s;
    for(i=0; i < nr1; i++)
        for(j=0; j < nc2; j++)
            {
            s = 0;
            for(k=0; k < nc1; k++)
                s += m1[i][k] * m2[k][j];
            dst[i][j] = s;
            }
    }

void mat_mulby(mat_t m1, mat_t m2, size_t nr1, size_t nc1, size_t nc2) /* nr1=nc1 */
/* m1 = m1 * m2  (in place)
 * (nr1 x nc2) = (nr1 x nc1) *(nc1 x nc2)
 */
    {
    size_t i, j, k;
    double s;
    mat_t tmp;
    mat_copy(tmp, m1);
    for(i=0; i < nr1; i++)
        for(j=0; j < nc2; j++)
            {
            s = 0;
            for(k=0; k < nc1; k++)
                s += tmp[i][k] * m2[k][j];
            m1[i][j] = s;
            }
    }


void mat_mxv(vec_t dst, mat_t m, vec_t v, size_t nr, size_t nc)
/* m is an nr x nc matrix
 * v is an nc column vector
 * dst is an nr column vector  (nr x nc) * (nc x 1) = (nr x 1)
 */
    {
    size_t i, j;
    double s;
    for(i=0; i < nr; i++)
        {
        s = 0;
        for(j=0; j < nc; j++)
            s += (m[i][j] * v[j]);
        dst[i] = s;
        }
    }

void mat_vxm(vec_t dst, vec_t v, mat_t m, size_t nr, size_t nc)
/* v is an nr row vector
 * m is an nr x nc matrix
 * dst is an nc row vector  (1 x nr) x (nr x nc) = (1 x nc)
 */
    {
    size_t i, j;
    double s;
    for(i=0; i < nc; i++)
        {
        s = 0;
        for(j=0; j < nr; j++)
            s += (v[j] * m[j][i]);
        dst[i] = s;
        }
    }

void mat_adj(mat_t dst, mat_t m, size_t n)
    {
    if(n==2)
        {
        dst[0][0] = m[1][1];
        dst[0][1] = -m[0][1];
        dst[1][0] = -m[1][0];
        dst[1][1] = m[0][0];
        }
    else if(n==3)
        {
        dst[0][0] = det2_(m[1][1], m[1][2], m[2][1], m[2][2]);
        dst[0][1] = det2_(m[0][2], m[0][1], m[2][2], m[2][1]);
        dst[0][2] = det2_(m[0][1], m[0][2], m[1][1], m[1][2]);
        dst[1][0] = det2_(m[1][2], m[1][0], m[2][2], m[2][0]);
        dst[1][1] = det2_(m[0][0], m[0][2], m[2][0], m[2][2]);
        dst[1][2] = det2_(m[0][2], m[0][0], m[1][2], m[1][0]);
        dst[2][0] = det2_(m[1][0], m[1][1], m[2][0], m[2][1]);
        dst[2][1] = det2_(m[0][1], m[0][0], m[2][1], m[2][0]);
        dst[2][2] = det2_(m[0][0], m[0][1], m[1][0], m[1][1]);
        }
    else //if(nr==4)
        {
        dst[0][0]=det3_(m[1][1],m[1][2],m[1][3],m[2][1],m[2][2],m[2][3],m[3][1],m[3][2],m[3][3]);
        dst[0][1]=-det3_(m[0][1],m[0][2],m[0][3],m[2][1],m[2][2],m[2][3],m[3][1],m[3][2],m[3][3]);
        dst[0][2]=det3_(m[0][1],m[0][2],m[0][3],m[1][1],m[1][2],m[1][3],m[3][1],m[3][2],m[3][3]);
        dst[0][3]=-det3_(m[0][1],m[0][2],m[0][3],m[1][1],m[1][2],m[1][3],m[2][1],m[2][2],m[2][3]);
        dst[1][0]=-det3_(m[1][0],m[1][2],m[1][3],m[2][0],m[2][2],m[2][3],m[3][0],m[3][2],m[3][3]);
        dst[1][1]=det3_(m[0][0],m[0][2],m[0][3],m[2][0],m[2][2],m[2][3],m[3][0],m[3][2],m[3][3]);
        dst[1][2]=-det3_(m[0][0],m[0][2],m[0][3],m[1][0],m[1][2],m[1][3],m[3][0],m[3][2],m[3][3]);
        dst[1][3]=det3_(m[0][0],m[0][2],m[0][3],m[1][0],m[1][2],m[1][3],m[2][0],m[2][2],m[2][3]);
        dst[2][0]=det3_(m[1][0],m[1][1],m[1][3],m[2][0],m[2][1],m[2][3],m[3][0],m[3][1],m[3][3]);
        dst[2][1]=-det3_(m[0][0],m[0][1],m[0][3],m[2][0],m[2][1],m[2][3],m[3][0],m[3][1],m[3][3]);
        dst[2][2]=det3_(m[0][0],m[0][1],m[0][3],m[1][0],m[1][1],m[1][3],m[3][0],m[3][1],m[3][3]);
        dst[2][3]=-det3_(m[0][0],m[0][1],m[0][3],m[1][0],m[1][1],m[1][3],m[2][0],m[2][1],m[2][3]);
        dst[3][0]=-det3_(m[1][0],m[1][1],m[1][2],m[2][0],m[2][1],m[2][2],m[3][0],m[3][1],m[3][2]);
        dst[3][1]=det3_(m[0][0],m[0][1],m[0][2],m[2][0],m[2][1],m[2][2],m[3][0],m[3][1],m[3][2]);
        dst[3][2]=-det3_(m[0][0],m[0][1],m[0][2],m[1][0],m[1][1],m[1][2],m[3][0],m[3][1],m[3][2]);
        dst[3][3]=det3_(m[0][0],m[0][1],m[0][2],m[1][0],m[1][1],m[1][2],m[2][0],m[2][1],m[2][2]);
        }
    }

int mat_inv(mat_t dst, mat_t m, size_t n)
    {
    size_t i, j;
    double d;
    if(n==2)
        d = det2(m);
    else if(n==3)
        d = det3(m);
    else //if(n==4)
        d = det4(m);
    if(d == 0.0)
        return 0;
    mat_adj(dst, m, n);
    for(i=0; i<n; i++)
        for(j=0; j<n; j++)
            dst[i][j] /= d;
    return 1;
    }

int mat_clamp(mat_t dst, mat_t m, mat_t minm, mat_t maxm, size_t nr, size_t nc)
    {
    size_t i, j;
    for(i = 0; i < nr; i++)
        for(j = 0; j < nc; j++)
            dst[i][j] = clamp(m[i][j], minm[i][j], maxm[i][j]);
    return 1;
    }

int mat_mix(mat_t dst, mat_t m1, mat_t m2, size_t nr, size_t nc, double k)
    {
    size_t i, j;
    for(i = 0; i < nr; i++)
        for(j = 0; j < nc; j++)
            dst[i][j] = mix(m1[i][j], m2[i][j], k);
    return 1;
    }

int mat_step(mat_t dst, mat_t m, mat_t edge, size_t nr, size_t nc)
    {
    size_t i, j;
    for(i = 0; i < nr; i++)
        for(j = 0; j < nc; j++)
            dst[i][j] = step(m[i][j], edge[i][j]);
    return 1;
    }

int mat_smoothstep(mat_t dst, mat_t m, mat_t edge0, mat_t edge1, size_t nr, size_t nc)
    {
    size_t i, j;
    for(i = 0; i < nr; i++)
        for(j = 0; j < nc; j++)
            dst[i][j] = smoothstep(m[i][j], edge0[i][j], edge1[i][j]);
    return 1;
    }

int mat_fade(mat_t dst, mat_t m, mat_t edge0, mat_t edge1, size_t nr, size_t nc)
    {
    size_t i, j;
    for(i = 0; i < nr; i++)
        for(j = 0; j < nc; j++)
            dst[i][j] = fade(m[i][j], edge0[i][j], edge1[i][j]);
    return 1;
    }


/*------------------------------------------------------------------------------*
 | Metamethods                                                                  |
 *------------------------------------------------------------------------------*/

static int Unm(lua_State *L)
    {
    size_t i, j, nr, nc;
    mat_t m;
    checkmat(L, 1, m, &nr, &nc);
    for(i = 0; i < nr; i++)
        for(j = 0; j < nc; j++)
            m[i][j] = -m[i][j];
    return pushmat(L, m, nr, nc, nr, nc);
    }

static int Add(lua_State *L)
    {
    size_t i, j, nr, nc, nr1, nc1;
    mat_t m, m1;
    checkmat(L, 1, m, &nr, &nc);
    checkmat(L, 2, m1, &nr1, &nc1);
    if((nr!=nr1) || (nc!=nc1))
        return luaL_error(L, OPERANDS_ERROR);
    for(i = 0; i < nr; i++)
        for(j = 0; j < nc; j++)
            m[i][j] += m1[i][j];
    return pushmat(L, m, nr, nc, nr, nc);
    }

static int Sub(lua_State *L)
    {
    size_t i, j, nr, nc, nr1, nc1;
    mat_t m, m1;
    checkmat(L, 1, m, &nr, &nc);
    checkmat(L, 2, m1, &nr1, &nc1);
    if((nr!=nr1) || (nc!=nc1))
        return luaL_error(L, OPERANDS_ERROR);
    for(i = 0; i < nr; i++)
        for(j = 0; j < nc; j++)
            m[i][j] -= m1[i][j];
    return pushmat(L, m, nr, nc, nr, nc);
    }

static int Mxs(lua_State *L, int sarg, int marg)
    {
    size_t i, j, nr, nc;
    mat_t m;
    double s = luaL_checknumber(L, sarg);
    checkmat(L, marg, m, &nr, &nc);
    for(i = 0; i < nr; i++)
        for(j = 0; j < nc; j++)
            m[i][j] *= s;
    return pushmat(L, m, nr, nc, nr, nc);
    }

static int Mxv(lua_State *L) /* MxN * Nx1 = N*1 (column vector)*/
    {
    unsigned int isrow;
    size_t i, j, nr, nc, size;
    mat_t m;
    vec_t v, v1;
    double s;
    checkmat(L, 1, m, &nr, &nc);
    checkvec(L, 2, v, &size, &isrow);
    if( isrow || (size != nc))
        {
#if 0
        if((size == 3) && (nr == 4) && (nc == 4))
            {
            /* Keep only the upper left 3x3 matrix */
            nr = nc = 3;
            }
        else
#endif
            return luaL_error(L, OPERANDS_ERROR);
        }
    for(i=0; i < nr; i++)
        {
        s = 0;
        for(j=0; j < size; j++)
            s += (m[i][j] * v[j]);
        v1[i] = s;
        }
    return pushvec(L, v1, size, size, 0);
    }


static int Mul(lua_State *L)
    {
    size_t i, j, k, nr1, nc1, nr2, nc2;
    mat_t m, m1, m2;
    double s;
    if(lua_isnumber(L, 1))
        return Mxs(L, 1, 2);
    if(lua_isnumber(L, 2))
        return Mxs(L, 2, 1);
    if(testvec(L, 2, NULL, NULL, NULL))
        return Mxv(L);
    /* matrix product  NxK * KxM -> NxM  (with N,K,M  != 1) */
    checkmat(L, 1, m1, &nr1, &nc1);
    checkmat(L, 2, m2, &nr2, &nc2);
    if((nc1 != nr2))
        return luaL_error(L, OPERANDS_ERROR);
    for(i=0; i < nr1; i++)
        for(j=0; j < nc2; j++)
            {
            s = 0;
            for(k=0; k < nc1; k++)
                s += m1[i][k] * m2[k][j];
            m[i][j] = s;
            }
    pushmat(L, m, nr1, nc2, nr1, nc2);
    return 1;
    }

static int Div(lua_State *L)
    {
    size_t i, j, nr, nc;
    mat_t m;
    double s;
    checkmat(L, 1, m, &nr, &nc);
    s = luaL_checknumber(L, 2);
    for(i = 0; i < nr; i++)
        for(j = 0; j < nc; j++)
            m[i][j] /= s;
    return pushmat(L, m, nr, nc, nr, nc);
    }

static int Pow(lua_State *L)
    {
    size_t nr, nc;
    mat_t m, dst;
    lua_Integer n, i;
    checkmat(L, 1, m, &nr, &nc);
    if((nc != nr))
        return luaL_error(L, OPERANDS_ERROR);
    n = luaL_checkinteger(L, 2);
    if(n==0) /* m^0 = identity */
        {
        mat_clear(dst);
        dst[0][0] = dst[1][1] = dst[2][2] = dst[3][3]= 1;
        }
    else 
        {
        if(n > 0)
            mat_copy(dst, m);
        else /* n < 0 */
            { 
            if(!mat_inv(dst, m, nr))
                return luaL_argerror(L, 1, "singular matrix");
            mat_copy(m, dst); 
            n = -n; 
            }
        for(i = 0; i < (n-1); i++)
            mat_mulby(dst, m, nr, nr, nr);
        }
    return pushmat(L, dst, nr, nc, nr, nc);
    }



int mat_Det(lua_State *L)
    {
    size_t nr, nc;
    mat_t m;
    checkmat(L, 1, m, &nr, &nc);
    if(nr != nc)
        return luaL_argerror(L, 1, "not a square matrix");
    if(nr==2)
        lua_pushnumber(L, det2(m));
    else if(nr==3)
        lua_pushnumber(L, det3(m));
    else //if(nr==4)
        lua_pushnumber(L, det4(m));
    return 1;
    }

int mat_Adj(lua_State *L)
    {
    size_t nr, nc;
    mat_t m, adj;
    checkmat(L, 1, m, &nr, &nc);
    if(nr != nc)
        return luaL_argerror(L, 1, "not a square matrix");
    mat_adj(adj, m, nr);
    return pushmat(L, adj, nr, nr, nr, nr);
    }

int mat_Inv(lua_State *L)
    {
    size_t nr, nc;
    mat_t m, inv;
    checkmat(L, 1, m, &nr, &nc);
    if(nr != nc)
        return luaL_argerror(L, 1, "not a square matrix");
    if(!mat_inv(inv, m, nr))
        return luaL_argerror(L, 1, "singular matrix");
    return pushmat(L, inv, nr, nr, nr, nr);
    }


int mat_Trace(lua_State *L)
    {
    size_t nr, nc, i;
    mat_t m;
    double res = 0;
    checkmat(L, 1, m, &nr, &nc);
    if(nr != nc)
        return luaL_argerror(L, 1, "not a square matrix");
    for(i = 0; i < nr; i++)
        res += m[i][i];
    lua_pushnumber(L, res);
    return 1;
    }


int mat_Transpose(lua_State *L)
    {
    size_t i, j, nr, nc;
    mat_t m, m1;
    checkmat(L, 1, m, &nr, &nc);
    for(i = 0; i < nr; i++)
        for(j = 0; j < nc; j++)
            m1[j][i] = m[i][j];
    return pushmat(L, m1, nc, nr, nc, nr);
    }

static int mat_Quat(lua_State *L)
    {
    return quat_FromMat(L);
    }

int mat_Column(lua_State *L)
    {
    mat_t m;
    vec_t v;
    size_t i, j, nr, nc;
    checkmat(L, 1, m, &nr, &nc);
    j = luaL_checkinteger(L, 2);
    if(j < 1 || j > nc)
        return luaL_argerror(L, 2, "invalid column index");
    j--;
    for(i = 0; i < nr; i++)
        v[i] = m[i][j];
    return pushvec(L, v, nr, nr, 0);
    }

int mat_Row(lua_State *L)
    {
    mat_t m;
    vec_t v;
    size_t i, j, nr, nc;
    checkmat(L, 1, m, &nr, &nc);
    i = luaL_checkinteger(L, 2);
    if(i < 1 || i > nr)
        return luaL_argerror(L, 2, "invalid row index");
    i--;
    for(j = 0; j < nc; j++)
        v[j] = m[i][j];
    return pushvec(L, v, nc, nc, 1);
    }

int mat_Clamp(lua_State *L)
    {
    mat_t dst;
    mat_t m, minm, maxm;
    size_t nr, nc, nr1, nc1;
    checkmat(L, 1, m, &nr, &nc);
    checkmat(L, 2, minm, &nr1, &nc1);
    if(nr1 != nr || nc1 != nc)
        return luaL_error(L, OPERANDS_ERROR);
    checkmat(L, 3, maxm, &nr1, &nc1);
    if(nr1 != nr || nc1 != nc)
        return luaL_error(L, OPERANDS_ERROR);
    mat_clamp(dst, m, minm, maxm, nr, nc);
    return pushmat(L, dst, nr, nc, nr, nc);
    }

int mat_Mix(lua_State *L)
    {
    mat_t dst, m1, m2;
    double k;
    size_t nr, nc, nr1, nc1;
    checkmat(L, 1, m1, &nr, &nc);
    checkmat(L, 2, m2, &nr1, &nc1);
    if(nr1 != nr || nc1 != nc)
        return luaL_error(L, OPERANDS_ERROR);
    k = luaL_checknumber(L, 3);
    mat_mix(dst, m1, m2, nr, nc, k);
    return pushmat(L, dst, nr, nc, nr, nc);
    }

int mat_Step(lua_State *L)
    {
    mat_t dst, m1, edge;
    size_t nr, nc, nr1, nc1, i, j;
    checkmat(L, 1, m1, &nr, &nc);
    if(lua_isnumber(L, 2))
        {
        double k = lua_tonumber(L, 2);
        for(i=0; i<nr; i++)
            for(j=0; j<nc; j++)
                edge[i][j] = k;
        }
    else
        {
        checkmat(L, 2, edge, &nr1, &nc1);
        if(nr1 != nr || nc1 != nc)
            return luaL_error(L, OPERANDS_ERROR);
        }
    mat_step(dst, m1, edge, nr, nc);
    return pushmat(L, dst, nr, nc, nr, nc);
    }

int mat_Smoothstep(lua_State *L)
    {
    mat_t dst, m, edge0, edge1;
    size_t nr, nc, nr1, nc1, i, j;
    checkmat(L, 1, m, &nr, &nc);
    if(lua_isnumber(L, 2))
        {
        double k = lua_tonumber(L, 2);
        for(i=0; i<nr; i++)
            for(j=0; j<nc; j++)
                edge0[i][j] = k;
        }
    else
        {
        checkmat(L, 2, edge0, &nr1, &nc1);
        if(nr1 != nr || nc1 != nc)
            return luaL_error(L, OPERANDS_ERROR);
        }
    if(lua_isnumber(L, 3))
        {
        double k = lua_tonumber(L, 3);
        for(i=0; i<nr; i++)
            for(j=0; j<nc; j++)
                edge1[i][j] = k;
        }
    else
        {
        checkmat(L, 3, edge1, &nr1, &nc1);
        if(nr1 != nr || nc1 != nc)
            return luaL_error(L, OPERANDS_ERROR);
        }
    mat_smoothstep(dst, m, edge0, edge1, nr, nc);
    return pushmat(L, dst, nr, nc, nr, nc);
    }

int mat_Fade(lua_State *L)
    {
    mat_t dst, m, edge0, edge1;
    size_t nr, nc, nr1, nc1, i, j;
    checkmat(L, 1, m, &nr, &nc);
    if(lua_isnumber(L, 2))
        {
        double k = lua_tonumber(L, 2);
        for(i=0; i<nr; i++)
            for(j=0; j<nc; j++)
                edge0[i][j] = k;
        }
    else
        {
        checkmat(L, 2, edge0, &nr1, &nc1);
        if(nr1 != nr || nc1 != nc)
            return luaL_error(L, OPERANDS_ERROR);
        }
    if(lua_isnumber(L, 3))
        {
        double k = lua_tonumber(L, 3);
        for(i=0; i<nr; i++)
            for(j=0; j<nc; j++)
                edge1[i][j] = k;
        }
    else
        {
        checkmat(L, 3, edge1, &nr1, &nc1);
        if(nr1 != nr || nc1 != nc)
            return luaL_error(L, OPERANDS_ERROR);
        }
    mat_fade(dst, m, edge0, edge1, nr, nc);
    return pushmat(L, dst, nr, nc, nr, nc);
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
//@@ TODO: norm, normalize
        { "adj", mat_Adj },
        { "det", mat_Det },
        { "inv", mat_Inv },
        { "transpose", mat_Transpose },
        { "trace", mat_Trace },
        { "quat", mat_Quat },
        { "column", mat_Column },
        { "row", mat_Row },
        { "clamp", mat_Clamp },
        { "mix", mat_Mix },
        { "step", mat_Step },
        { "smoothstep", mat_Smoothstep },
        { "fade", mat_Fade },
        { NULL, NULL } /* sentinel */
    };


/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "mat2", Mat2 },
        { "mat3", Mat3 },
        { "mat4", Mat4 },
        { "mat2x3", Mat2x3 },
        { "mat3x2", Mat3x2 },
        { "mat2x4", Mat2x4 },
        { "mat4x2", Mat4x2 },
        { "mat3x4", Mat3x4 },
        { "mat4x3", Mat4x3 },
        { "ismat2", IsMat2 },
        { "ismat3", IsMat3 },
        { "ismat4", IsMat4 },
        { "ismat2x3", IsMat2x3 },
        { "ismat3x2", IsMat3x2 },
        { "ismat2x4", IsMat2x4 },
        { "ismat4x2", IsMat4x2 },
        { "ismat3x4", IsMat3x4 },
        { "ismat4x3", IsMat4x3 },
        { NULL, NULL } /* sentinel */
    };

void moonglmath_open_mat(lua_State *L)
    {
    newmetatable(L, MAT_MT);
    metatable_setfuncs(L, MAT_MT, Metamethods, Methods);
    luaL_setfuncs(L, Functions, 0);
    }

