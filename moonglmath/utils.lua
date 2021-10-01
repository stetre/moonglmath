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
-- Adds named-components access to MoonGLMATH boxes
-- ( v.minx becomes an alias for b[1], v.maxx becomes an alias for b[2], and so on).
--

--local glmath = require("moonglmath")
--local mt = getmetatable(glmath.box2())
do
local glmath = moonglmath

local function dumpdata(data, filename, mode)
-- Dumps data in a file to be used with gnuplot
-- (eg. gnuplot> plot "data.log" using 1:2 with lines)
-- 'data' = sequence of numbers (array without gaps starting from index=1)
-- 'filename'= name of file where to dump the data
   local file = assert(io.open(filename, mode or "w"))
   for i,v in ipairs(data) do
      file:write(string.format("%u %.9f\n",i,v))
   end
   file:close()
end

local function stats(data)
-- Computes mean, variax, minimum and maximum over
-- the sequence of numbers contained in data
   local max,min = 0, math.huge
   local mean, delta, m2, var = 0, 0, 0, 0
   for i=1,#data do 
      local d = data[i]
      if d < min then min = d end
      if d > max then max = d end
      delta = d - mean
      mean = mean + delta/i
      m2 = m2 + delta*(d-mean)
   end
   if #data > 1 then
      var = m2 / (#data-1)
   end
   return  mean, var, min, max
end

moonglmath.stats = stats
moonglmath.dumpdata = dumpdata

end
