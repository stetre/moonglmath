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
 | Camera positioning (view matrix)                                             |
 *------------------------------------------------------------------------------*/

static int LookAt(lua_State *L)
/* look_at(eye, at, up) 
 *
 * eye = camera position = (e1, e2, e3)
 * at  = target position
 * up  = camera 'up' direction
 *
 * NOTE: 'up' is the direction from the camera to a point above the target,
 *        on the vertical.
 *
 * d = (at - eye) : direction vector
 * n = (eye - at) = z : normal to the projection plane, oriented towards the camera
 * x = d % up = up % n : points to the right of the camera
 * y = z % x = n % x : points up fro the camera
 *  
 * M = |x1 x2 x3 0| |1 0 0 -e1|
 *     |y1 y2 y3 0|·|0 1 0 -e2|
 *     |z1 z2 z3 0| |0 0 1 -e3|
 *     |0  0  0  1| |0 0 0   1|
 *          |           |
 *       orient     translate
 */
    {
    size_t i;
    vec_t eye, at, up, z, x, y;
    mat_t m, c, te;

    checkvec(L, 1, eye, NULL, NULL);
    checkvec(L, 2, at, NULL, NULL);
    checkvec(L, 3, up, NULL, NULL);

    vec_sub(z, eye, at, 4); /* z = eye - at (normal vector = - direction vector) */
    vec_normalize(z, 4);

    vec_cross(x, up, z); /* x = up % z  (right vector) */
    vec_normalize(x, 4);

    vec_cross(y, z, x);  /* y = z % x  (up vector) */
    vec_normalize(y, 4);

    mat_clear(c);   
    /* c = x1 x2 x3  0
     *     y1 y2 y3  0
     *     z1 z2 z3  0
     *      0  0  0  1
     */
    for(i = 0; i< 4; i ++)
        {
        c[0][i] = x[i];
        c[1][i] = y[i];
        c[2][i] = z[i];
        }
    c[3][3] = 1;

    /* m = c * translate(-eye) */
    translate(te, -eye[0], -eye[1], -eye[2]);
    mat_mul(m, c, te, 4, 4, 4);
    pushmat(L, m, 4, 4, 4, 4);

    return 1;
    }
/*------------------------------------------------------------------------------*
 | Projections                                                                  |
 *------------------------------------------------------------------------------*/

static int Ortho(lua_State *L)
/* ortho(left, right, bottom, top, near,  far)
 * near=-1 and far=1 give a 2D projection
 *
 * The viewing parallelepiped is: 
 * x: left .. right
 * y: bottom .. up
 * z: -near .. -far
 *
 * The camera is positioned at the origin, pointing to the -z axis (x=right, y=up)
 * and the projection plane is the x-y plane (or, equivalently, any plane parallel
 * to x-y, since the projectors are parallel to the z axis).
 *
 * M =  |2/(r-l)  0    0     0| |1 0 0 -(l+r)/2| = |2/(r-l)  0     0    -(r+l)/(r-l)|
 *      |  0   2/(t-b) 0     0|*|0 1 0 -(t+b)/2|   |  0   2/(t-b)  0    -(t+b)/(t-b)|
 *      |  0      0  2/(f-n) 0| |0 0 1 -(f+n)/2|   |  0     0   2/(f-n) -(f+n)/(f-n)|
 *      |  0      0    0     0| |0 0 0     1   |   |  0     0      0         1      |
 *           scaling               translation
 */
    {
    mat_t m;
    lua_Number l = luaL_checknumber(L, 1); /* left */
    lua_Number r = luaL_checknumber(L, 2); /* right */
    lua_Number b = luaL_checknumber(L, 3); /* bottom */
    lua_Number t = luaL_checknumber(L, 4); /* top */
    lua_Number n = luaL_checknumber(L, 5); /* near */
    lua_Number f = luaL_checknumber(L, 6); /* far */
    mat_clear(m);
    m[0][0] = 2.0/(r-l);
    m[0][3] = -(r+l)/(r-l);
    m[1][1] = 2.0/(t-b);
    m[1][3] = -(t+b)/(t-b);
    m[2][2] = -2.0/(f-n);
    m[2][3] = -(f+n)/(f-n);
    m[3][3] = 1.0;
    return pushmat(L, m, 4, 4, 4, 4);
    }


static int Frustum(lua_State *L)
/* frustum(left, right, bottom, top, near, far)
 */
    {
    mat_t m;
    lua_Number l = luaL_checknumber(L, 1); /* left */
    lua_Number r = luaL_checknumber(L, 2); /* right */
    lua_Number b = luaL_checknumber(L, 3); /* bottom */
    lua_Number t = luaL_checknumber(L, 4); /* top */
    lua_Number n = luaL_checknumber(L, 5); /* near */
    lua_Number f = luaL_checknumber(L, 6); /* far */
    mat_clear(m);
    m[0][0] = 2.0*n/(r-l);
    m[0][2] = (r+l)/(r-l);
    m[1][1] = 2.0*n/(t-b);
    m[1][2] = (t+b)/(t-b);
    m[2][2] = -(f+n)/(f-n);
    m[2][3] = -2.0*f*n/(n-f);
    m[3][2] = -1.0;
    return pushmat(L, m, 4, 4, 4, 4);
    }


static int Perspective(lua_State *L)
/* perspective(fovy, aspect, near, far)
 */
    {
    mat_t m;
    lua_Number fovy = luaL_checknumber(L, 1); /* radians */
    lua_Number aspect = luaL_checknumber(L, 2);
    lua_Number n = luaL_checknumber(L, 3); /* near */
    lua_Number f = luaL_checknumber(L, 4); /* far */
    lua_Number t = tan(fovy/2) * n; /* top */
    lua_Number r = t * aspect; /* right */
    mat_clear(m);
    m[0][0] = n/r;
    m[1][1] = n/t;
    m[2][2] = -(f+n)/(f-n);
    m[2][3] = -2.0*f*n/(f-n);
    m[3][2] = -1.0;
    return pushmat(L, m, 4, 4, 4, 4);
    }


/*------------------------------------------------------------------------------*
 | Registration                                                                 |
 *------------------------------------------------------------------------------*/

static const struct luaL_Reg Functions[] = 
    {
        { "look_at", LookAt },
        { "ortho", Ortho },
        { "frustum", Frustum },
        { "perspective", Perspective },
        { NULL, NULL } /* sentinel */
    };

void moonglmath_open_viewing(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

