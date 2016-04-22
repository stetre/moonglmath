local mt = moonglmath_quat
local methods = mt.__index

local rd = {} -- read functions
local wr = {} -- write indices

-- position components
rd.w = function(self) return self[1] end
rd.x = function(self) return self[2] end
rd.y = function(self) return self[3] end
rd.z = function(self) return self[4] end
wr.w, wr.x, wr.y, wr.z = 1,2,3,4

mt.__index = function(self, key) 
	local f = rd[key]
	if f then return f(self) end
	return methods[key]
end

mt.__newindex = function(self, key, val)
	if type(key) == "number" then
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

