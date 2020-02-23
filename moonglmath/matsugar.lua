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

do
local mt = getmetatable(moonglmath.mat2())
local methods = mt.__index

local rd = {} -- read functions
local wr = {} -- write indices

-- position components
rd._11 = function(self) return self[1][1] end
rd._12 = function(self) return self[1][2] end
rd._13 = function(self) return self[1][3] end
rd._14 = function(self) return self[1][4] end
rd._21 = function(self) return self[2][1] end
rd._22 = function(self) return self[2][2] end
rd._23 = function(self) return self[2][3] end
rd._24 = function(self) return self[2][4] end
rd._31 = function(self) return self[3][1] end
rd._32 = function(self) return self[3][2] end
rd._33 = function(self) return self[3][3] end
rd._34 = function(self) return self[3][4] end
rd._41 = function(self) return self[4][1] end
rd._42 = function(self) return self[4][2] end
rd._43 = function(self) return self[4][3] end
rd._44 = function(self) return self[4][4] end

wr._11 = function() return 1, 1 end
wr._12 = function() return 1, 2 end
wr._13 = function() return 1, 3 end
wr._14 = function() return 1, 4 end
wr._21 = function() return 2, 1 end
wr._22 = function() return 2, 2 end
wr._23 = function() return 2, 3 end
wr._24 = function() return 2, 4 end
wr._31 = function() return 3, 1 end
wr._32 = function() return 3, 2 end
wr._33 = function() return 3, 3 end
wr._34 = function() return 3, 4 end
wr._41 = function() return 4, 1 end
wr._42 = function() return 4, 2 end
wr._43 = function() return 4, 3 end
wr._44 = function() return 4, 4 end


mt.__index = function(self, key) 
	local f = rd[key]
	if f then return f(self) end
	return methods[key]
end

mt.__newindex = function(self, key, val)
	local f = wr[key]
	if f then 
		local i,j = f()
		local row = self[i]
		rawset(row, j, val)
	else
		rawset(self, key, val)
	end
end

moonglmath.tomat2 = function(t) setmetatable(t, mt) t.rows=2 t.columns=2 return t end
moonglmath.tomat3 = function(t) setmetatable(t, mt) t.rows=3 t.columns=3 return t end
moonglmath.tomat4 = function(t) setmetatable(t, mt) t.rows=4 t.columns=4 return t end
moonglmath.tomat2x3 = function(t) setmetatable(t, mt) t.rows=2 t.columns=3 return t end
moonglmath.tomat3x2 = function(t) setmetatable(t, mt) t.rows=3 t.columns=2 return t end
moonglmath.tomat2x4 = function(t) setmetatable(t, mt) t.rows=2 t.columns=4 return t end
moonglmath.tomat4x2 = function(t) setmetatable(t, mt) t.rows=4 t.columns=2 return t end
moonglmath.tomat3x4 = function(t) setmetatable(t, mt) t.rows=3 t.columns=4 return t end
moonglmath.tomat4x3 = function(t) setmetatable(t, mt) t.rows=4 t.columns=3 return t end

end
