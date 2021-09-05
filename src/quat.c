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

int testquat(lua_State *L, int arg, quat_t q)
/* Tests if the element at arg is a quaternion and sets q accordingly
 * (unless it is passed as 0).
 */
    {
    size_t i;
    if(!testmetatable(L, arg, QUAT_MT)) return 0;

    if(q != NULL)
        {
        for(i=0; i < 4; i++)
            {
            lua_geti(L, arg, i+1);
            q[i] = luaL_checknumber(L, -1);
            lua_pop(L, 1);
            }
        }
    return 1;
    }

int checkquat(lua_State *L, int arg, quat_t q)
/* Same as testquat(), but raises a error if the test fails.
 */
    {
    if(!testquat(L, arg, q))
        return luaL_argerror(L, arg, lua_pushfstring(L, "%s expected", QUAT_MT));
    return 1;
    }


int pushquat(lua_State *L, quat_t q)
    {
    size_t i;
    lua_newtable(L);
    setmetatable(L, QUAT_MT);
    for(i=0; i<4; i++)
        {
        lua_pushnumber(L, q[i]);
        lua_seti(L, -2, i+1);
        }
    return 1;
    }

/*------------------------------------------------------------------------------*
 | Vec                                                                          |
 *------------------------------------------------------------------------------*/

static int Quat(lua_State *L)
/* creates a quaternion from another quaternion, a scalar and a vec3, a matrix,
 * or a list of scalars.
 * the size is adapted by discarding exceeding values or adding zeros in lieu 
 * of missing values.
 * quat(q)
 * quat([w [, x [, y [, z]]])
 * quat(s, v)
 * quat(mat)
 * quat(axis, theta)
 */
    {
    quat_t q;
    vec_t v;
    double halftheta, s;
    size_t vsize, i, arg;

    if(testquat(L, 1, q))
        return pushquat(L, q);

    if(testvec(L, 2, v, &vsize, NULL))
        {
#if 0
        if(vsize != 3)
            return luaL_argerror(L, 2, "size 3 vector expected");
#endif
        q[0] = luaL_checknumber(L, 1);
        q[1] = v[0];
        q[2] = v[1];
        q[3] = v[2];
        return pushquat(L, q);
        }

    if(testvec(L, 1, v, &vsize, NULL)) // rotation quaternion
        {
        halftheta = luaL_checknumber(L, 2)/2;
        s = sin(halftheta);
        q[0] = cos(halftheta);
        q[1] = v[0] * s;
        q[2] = v[1] * s;
        q[3] = v[2] * s;
        quat_normalize(q);
        return pushquat(L, q);
        }

    if(testmat(L, 1, NULL, NULL, NULL))
        return quat_FromMat(L);

    arg = 1;
    for(i = 0; i < 4; i++)
        q[i] = luaL_optnumber(L, arg++, 0);
    return pushquat(L, q);
    }


static int IsQuat(lua_State *L)
    {
    if(!testquat(L, 1, NULL))
        lua_pushboolean(L, 0);
    else
        lua_pushboolean(L, 1);
    return 1;
    }
    
/*------------------------------------------------------------------------------*
 | ToString                                                                     |
 *------------------------------------------------------------------------------*/

static int ToString_(lua_State *L, quat_t q)
/* With snprintf because lua_pushfstring does not support the %g conversion specifier */
    {
#define SZ 512
#define space (SZ-p)
    char str[SZ];
    int p=0;
    p = snprintf(str, space, "< "FMT", "FMT", "FMT", "FMT" >", q[0], q[1], q[2], q[3]);
    lua_pushlstring(L, str, p);
    return 1;
#undef space
#undef SZ
    }

static int ToString(lua_State *L)
    {
    quat_t q;
    checkquat(L, 1, q);
    return ToString_(L, q);
    }


static int Concat(lua_State *L)
    {
    quat_t q;
    if(testquat(L, 1, q))
        {
        ToString_(L, q);
        lua_pushvalue(L, 2);
        }
    else if(testquat(L, 2, q))
        {
        lua_pushvalue(L, 1);
        ToString_(L, q);
        }
    else 
        return unexpected(L);
    lua_concat(L, 2);
    return 1;
    }


/*------------------------------------------------------------------------------*
 | Non-Lua functions (for internal use)                                         |
 *------------------------------------------------------------------------------*/

void quat_unm(quat_t dst, quat_t q)
    {
    size_t i; 
    quat_clear(dst);
    for(i=0; i < 4; i++)    
        dst[i] = -q[i];
    }
    
void quat_add(quat_t dst, quat_t q1, quat_t q2)
    {
    size_t i; 
    for(i=0; i < 4; i++)    
        dst[i] = q1[i] + q2[i];
    }
    
void quat_sub(quat_t dst, quat_t q1, quat_t q2)
    {
    size_t i; 
    for(i=0; i < 4; i++)    
        dst[i] = q1[i] - q2[i];
    }

double quat_norm(quat_t q)
    {
    size_t i;
    double r = 0;
    for(i=0; i < 4; i++)
        r += q[i]*q[i];
    return sqrt(r);
    }

double quat_norm2(quat_t q)
    {
    size_t i;
    double r = 0;
    for(i=0; i < 4; i++)
        r += q[i]*q[i];
    return r;
    }

void quat_normalize(quat_t q) 
/* in place */
    {
    size_t i;
    double norm = quat_norm(q);
    for(i=0; i < 4; i++)    
        q[i] = q[i]/norm;
    }
        
void quat_conj(quat_t dst, quat_t q) 
    {
    dst[0] = q[0];
    dst[1] = -q[1];
    dst[2] = -q[2];
    dst[3] = -q[3];
    }


void quat_inv(quat_t dst, quat_t q) 
    {
    double norm2 = quat_norm(q);
    norm2 = norm2*norm2;
    dst[0] = q[0]/norm2;
    dst[1] = -q[1]/norm2;
    dst[2] = -q[2]/norm2;
    dst[3] = -q[3]/norm2;
    }

void quat_div(quat_t dst, quat_t q, double s)
    {
    size_t i; 
    for(i=0; i < 4; i++)    
        dst[i] = q[i] / s;
    }

void quat_mul(quat_t dst, quat_t q, quat_t p)
    {
    dst[0] = q[0]*p[0] - q[1]*p[1] - q[2]*p[2] - q[3]*p[3];
    dst[1] = q[0]*p[1] + q[1]*p[0] + q[2]*p[3] - q[3]*p[2];
    dst[2] = q[0]*p[2] + q[2]*p[0] + q[3]*p[1] - q[1]*p[3];
    dst[3] = q[0]*p[3] + q[3]*p[0] + q[1]*p[2] - q[2]*p[1];
    }

static void quat_mulby(quat_t dst, quat_t p) /* in place, dst = dst * q */
    {
    quat_t q;
    quat_copy(q, dst);  
    dst[0] = q[0]*p[0] - q[1]*p[1] - q[2]*p[2] - q[3]*p[3];
    dst[1] = q[0]*p[1] + q[1]*p[0] + q[2]*p[3] - q[3]*p[2];
    dst[2] = q[0]*p[2] + q[2]*p[0] + q[3]*p[1] - q[1]*p[3];
    dst[3] = q[0]*p[3] + q[3]*p[0] + q[1]*p[2] - q[2]*p[1];
    }


void quat_qxs(quat_t dst, quat_t q, double s)
    {
    size_t i; 
    for(i=0; i < 4; i++)    
        dst[i] = q[i] * s;
    }

void quat_mix(quat_t dst, quat_t q, quat_t p, double t)
    {
    double t1 = 1.0f - t;
    dst[0] = t1*q[0] + t*p[0];
    dst[1] = t1*q[1] + t*p[1];
    dst[2] = t1*q[2] + t*p[2];
    dst[3] = t1*q[3] + t*p[3];
    }

void quat_slerp(quat_t dst, quat_t q, quat_t p, double t)
/* Rfr: "3D Math Primer for Graphics and Game Development", by F. Dunn, I. Parberry,
 *      AK Peters/CRC Press, section 8.5.11.
 */
    {
    double c, s, theta, t0, t1;
    c = q[0]*p[0] + q[1]*p[1] + q[2]*p[2] + q[3]*p[3];
    if(c < 0.0f)
        {
        c = -c;
        p[0] = - p[0];
        p[1] = - p[1];
        p[2] = - p[2];
        p[3] = - p[3];
        }
    if(c > .9999f) // q and p are very close: use linear interpolation
        quat_mix(dst, q, p, t);
    else
        {
        s = sqrt(1.0f - c*c);
        theta = atan2(s, c);
        t0 = sin((1.0f - t)*theta)/s;
        t1 = sin(t*theta)/s;
        dst[0] = t0*q[0] + t1*p[0];
        dst[1] = t0*q[1] + t1*p[1];
        dst[2] = t0*q[2] + t1*p[2];
        dst[3] = t0*q[3] + t1*p[3];
        }
    }

/*------------------------------------------------------------------------------*
 | Metamethods                                                                  |
 *------------------------------------------------------------------------------*/

static int Unm(lua_State *L)
    {
    quat_t q; 
    size_t i; 
    checkquat(L, 1, q);
    for(i=0; i < 4; i++)    
        q[i] = -q[i];
    return pushquat(L, q);
    }


static int Add(lua_State *L)
    {
    quat_t q, q1; 
    size_t i; 
    checkquat(L, 1, q);
    checkquat(L, 2, q1);
    for(i=0; i < 4; i++)    
        q[i] += q1[i];
    return pushquat(L, q);
    }

static int Sub(lua_State *L)
    {
    quat_t q, q1; 
    size_t i; 
    checkquat(L, 1, q);
    checkquat(L, 2, q1);
    for(i=0; i < 4; i++)    
        q[i] -= q1[i];
    return pushquat(L, q);
    }

static int Qxs(lua_State *L, int sarg, int qarg)
    {
    quat_t q; 
    size_t i; 
    double s = luaL_checknumber(L, sarg);
    checkquat(L, qarg, q);
    for(i=0; i < 4; i++)    
        q[i] *= s;
    return pushquat(L, q);
    }

static int Mul(lua_State *L)
    {
    quat_t q, p, r; 
    if(lua_isnumber(L, 1))
        return Qxs(L, 1, 2);
    if(lua_isnumber(L, 2))
        return Qxs(L, 2, 1);
    checkquat(L, 1, q);
    checkquat(L, 2, p);
    r[0] = q[0]*p[0] - q[1]*p[1] - q[2]*p[2] - q[3]*p[3];
    r[1] = q[0]*p[1] + q[1]*p[0] + q[2]*p[3] - q[3]*p[2];
    r[2] = q[0]*p[2] + q[2]*p[0] + q[3]*p[1] - q[1]*p[3];
    r[3] = q[0]*p[3] + q[3]*p[0] + q[1]*p[2] - q[2]*p[1];
    return pushquat(L, r);
    }

static int Pow(lua_State *L)
    {
    quat_t q, dst; 
    lua_Integer n, i;
    checkquat(L, 1, q);
    n = luaL_checkinteger(L, 2);
    if(n == 0)
        {
        quat_clear(dst); dst[0]=1;  
        }
    else
        {
        if(n > 0)
            quat_copy(dst, q);
        else
            {
            quat_inv(dst, q);
            quat_copy(q, dst);
            n = -n;
            }
        for(i = 0; i < (n-1); i++)
            quat_mulby(dst, q);
        }
    return pushquat(L, dst);
    }


static int Div(lua_State *L)
    {
    quat_t q; 
    size_t i; 
    double s;
    checkquat(L, 1, q);
    s = luaL_checknumber(L, 2);
    for(i=0; i < 4; i++)    
        q[i] /= s;
    return pushquat(L, q);
    }

int quat_Norm(lua_State *L)
    {
    quat_t q;
    size_t i;
    double r = 0;
    checkquat(L, 1, q);
    for(i=0; i < 4; i++)
        r += q[i]*q[i];
    lua_pushnumber(L, sqrt(r));
    return 1;
    }

int quat_Norm2(lua_State *L)
    {
    quat_t q;
    size_t i;
    double r = 0;
    checkquat(L, 1, q);
    for(i=0; i < 4; i++)
        r += q[i]*q[i];
    lua_pushnumber(L, r);
    return 1;
    }

int quat_Conj(lua_State *L)
    {
    quat_t q; 
    checkquat(L, 1, q);
    q[1] = -q[1];
    q[2] = -q[2];
    q[3] = -q[3];
    return pushquat(L, q);
    }

int quat_Normalize(lua_State *L)
    {
    quat_t q; 
    checkquat(L, 1, q);
    quat_normalize(q);
    return pushquat(L, q);
    }

int quat_Inv(lua_State *L)
    {
    quat_t q, inv;
    checkquat(L, 1, q);
    quat_inv(inv, q);
    return pushquat(L, inv);
    }

int quat_Parts(lua_State *L)
    {
    quat_t q; 
    vec_t v;
    checkquat(L, 1, q);
    v[0] = q[1];
    v[1] = q[2];
    v[2] = q[3];
    lua_pushnumber(L, q[0]);
    pushvec(L, v, 3, 3, 0);
    return 2;
    }

/*--------------------------------------------------------------------------*
 | quaternion <-> rotation matrix conversion                                |
 *--------------------------------------------------------------------------*/
/* Rfr: https://en.wikipedia.org/wiki/Rotation_matrix#Quaternion */

static int quat_Mat(lua_State *L, size_t n)
/* returns the rotation matrix corresponding to the quaternion */
    {
#define w q[0]
#define x q[1]
#define y q[2]
#define z q[3]
    quat_t q; 
    mat_t m;
    double s, wx, wy, wz, xx, yy, zz, xy, xz, yz;
    checkquat(L, 1, q);
    mat_clear(m);
    s = w*w + x*x + y*y +z*z;
    if(s == 0)
        {
        m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
        return pushmat(L, m, 4, 4, n, n);
        }
    s = 2 / s;  
    wx = s*w*x;
    wy = s*w*y;
    wz = s*w*z;
    xx = s*x*x;
    yy = s*y*y;
    zz = s*z*z;
    xy = s*x*y;
    xz = s*x*z;
    yz = s*y*z;
    
    m[0][0] = 1 - yy - zz;
    m[1][1] = 1 - xx - zz;
    m[2][2] = 1 - xx - yy;
    m[3][3] = 1;
    m[0][1] = xy - wz;
    m[1][0] = xy + wz;
    m[0][2] = xz + wy;
    m[2][0] = xz - wy;
    m[1][2] = yz - wx;
    m[2][1] = yz + wx;
    return pushmat(L, m, 4, 4, n, n);
#undef w
#undef x
#undef y
#undef z
    }

static int quat_Mat3(lua_State *L)
    { return quat_Mat(L, 3); }

static int quat_Mat4(lua_State *L)
    { return quat_Mat(L, 4); }


int quat_FromMat(lua_State *L)
    {
    size_t nr, nc;
    double t, s, r;
    mat_t m;
    quat_t q;
    checkmat(L, 1, m, &nr, &nc);
    if( !(((nr==3) && (nc==3)) || ((nr==4) && (nc==4))) )
        return luaL_argerror(L, 1, "expected 3x3 or 4x4 matrix");
    t = m[0][0] + m[1][1] + m[2][2];
    if( t < 0 )
        {
        r = sqrt(1 + m[0][0] - m[1][1] - m[2][2]);
        s = 0.5/r;
        q[0] = (m[2][1] - m[1][2])*s;
        q[1] = 0.5*r;
        q[2] = (m[0][1] + m[1][0])*s;
        q[3] = (m[2][0] + m[0][2])*s;
        }
    else
        {
        r = sqrt(1+t);
        s = 0.5/r;
        q[0] = 0.5*r;
        q[1] = (m[2][1] - m[1][2])*s;
        q[2] = (m[0][2] - m[2][0])*s;
        q[3] = (m[1][0] + m[0][1])*s;
        }
    return pushquat(L, q);
    }


int quat_Mix(lua_State *L)
    {
    quat_t dst, q, p;
    double t;
    checkquat(L, 1, q);
    checkquat(L, 2, p);
    t = luaL_checknumber(L, 3);
    quat_mix(dst, q, p, t);
    return pushquat(L, dst);
    }

int quat_Slerp(lua_State *L)
    {
    quat_t dst, q, p;
    double t;
    checkquat(L, 1, q);
    checkquat(L, 2, p);
    t = luaL_checknumber(L, 3);
    quat_slerp(dst, q, p, t);
    return pushquat(L, dst);
    }


//@@ TODO rotate()
//@@ TODO quaternion <-> Euler

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
        { "parts", quat_Parts },
        { "conj", quat_Conj },
        { "norm", quat_Norm },
        { "norm2", quat_Norm2 },
        { "normalize", quat_Normalize },
        { "inv", quat_Inv },
        { "mat3", quat_Mat3 },
        { "mat4", quat_Mat4 },
        { "mix", quat_Mix },
        { "slerp", quat_Slerp },
        { NULL, NULL } /* sentinel */
    };


/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "quat", Quat },
        { "isquat", IsQuat },
        { NULL, NULL } /* sentinel */
    };

void moonglmath_open_quat(lua_State *L)
    {
    newmetatable(L, QUAT_MT);
    metatable_setfuncs(L, QUAT_MT, Metamethods, Methods);
    luaL_setfuncs(L, Functions, 0);
    }

