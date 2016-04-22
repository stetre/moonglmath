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

#ifndef enumDEFINED
#define enumDEFINED

/*--------------------------------------------------------------------------*
 | unsigned int <-> string mappings                                             |
 *--------------------------------------------------------------------------*/

#define enum_t moonglmath_enum_t
typedef struct {
    const char **strings;
    unsigned int *codes;
    size_t n;
} enum_t;

#define enumCheck moonglmath_enumCheck
unsigned int enumCheck(lua_State *L, int arg, enum_t *e);
#define enumPush moonglmath_enumPush
int enumPush(lua_State *L, unsigned int code, enum_t *e);
#define enumToString moonglmath_enumToString
const char* enumToString(unsigned int code, enum_t *e);
#define enumAssert moonglmath_enumAssert
int enumAssert(lua_State *L, enum_t *e, unsigned int code, const char *string);
#define enumOrUint moonglmath_enumOrUint
unsigned int enumOrUint(lua_State *L, int arg, unsigned int *dst, enum_t *e, unsigned int def);

#define ENUM_STRINGS(strings)   static const char* (strings) []
#define ENUM_CODES(codes)       static unsigned int (codes) []
#define ENUM_T(name, strings, codes)    \
    static enum_t name = { (strings), (codes), sizeof((codes))/sizeof(unsigned int) };


#if 0
#define  moonglmath_
#endif

#if 0 /* @@ 11yy */
ENUM_STRINGS(ZzzStrings) = {
    "",
    NULL
};
ENUM_CODES(ZzzCodes) = {
    GL_,
};
ENUM_T(ZzzEnum, ZzzStrings, ZzzCodes)
#define CheckZzz(L, arg) enumCheck((L), (arg), &ZzzEnum)
#define PushZzz(L, code) enumPush((L), (code), &ZzzEnum)


#define ToStringZzz(code) enumToString((code), &ZzzEnum)

#define CheckZzzOrNnn(L, arg, dst) enumOrUint((L), (arg), (dst), &ZzzEnum, 0)

enum_t *enumZzz(void)
    { return &ZzzEnum; }

#define ZzzEnum yyyZzz()
#define CheckZzz(L, arg) enumCheck((L), (arg), ZzzEnum)
#define PushZzz(L, code) enumPush((L), (code), ZzzEnum)
#define ToStringZzz(code) enumToString((code), ZzzEnum)

#endif

#endif /* enumDEFINED */
