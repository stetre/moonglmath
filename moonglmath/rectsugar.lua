-- The MIT License (MIT)
--
-- Copyright (c) 2016 Stefano Trettel
--
-- Software repository: MoonGLMATH, https://github.com/stetre/moonglmath
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in all
-- copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
-- SOFTWARE.
--


-- 
-- Adds named-components access to MoonGLMATH rects
-- ( v.x becomes an alias for b[1], v.y becomes an alias for b[2], and so on).
--
do
local mt = getmetatable(moonglmath.rect())
local methods = mt.__index

local rd = {} -- read functions
local wr = {} -- write indices

-- position components
rd.x = function(self) return self[1] end
rd.y = function(self) return self[2] end
rd.w = function(self) return self[3] end
rd.h = function(self) return self[4] end
wr.x, wr.y, wr.w, wr.h = 1,2,3,4

mt.__index = function(self, key) 
   local f = rd[key]
   if f then return f(self) end
   return methods[key]
end

mt.__newindex = function(self, key, val)
   if type(key) == "number" or key == "dimensions" then
      rawset(self, key, val)
      return
   end
   local i = wr[key]
   if i then 
      rawset(self, i, val)
   else
      error("cannot write field '" .. key .."'", 2)
   end
end

moonglmath.torect = function(t) setmetatable(t, mt) return t end

end
