-- 
-- Adds named-components access to MoonGLMATH vectors
-- ( v.x becomes an alias for v[1], v.y becomes an alias for v[2], and so on).
--
--
--
--

--local glmath = require("moonglmath")
--local mt = getmetatable(glmath.vec2())
local mt = moonglmath_vec
local methods = mt.__index

local rd = {} -- read functions
local wr = {} -- write indices

-- position components
rd.x = function(self) return self[1] end
rd.y = function(self) return self[2] end
rd.z = function(self) return self[3] end
rd.w = function(self) return self[4] end
wr.x, wr.y, wr.z, wr.w = 1,2,3,4

-- color components
rd.r = function(self) return self[1] end
rd.g = function(self) return self[2] end
rd.b = function(self) return self[3] end
rd.a = function(self) return self[4] end
wr.r, wr.g, wr.b, wr.a = 1,2,3,4

-- texture components
rd.s = function(self) return self[1] end
rd.t = function(self) return self[2] end
rd.p = function(self) return self[3] end
rd.q = function(self) return self[4] end
wr.s, wr.t, wr.p, wr.q = 1,2,3,4

mt.__index = function(self, key) 
	local f = rd[key]
	if f then return f(self) end
	return methods[key]
end

mt.__newindex = function(self, key, val)
	if type(key) == "number" or key == "size" or key == "type" then
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

