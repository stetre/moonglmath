-- Interactive MoonGLMATH
--
-- This script just globalizes the contents of the glmath and math tables,
-- (except for math.type(), otherwise we would loose the Lua type() function)
-- so that we can call them without the table name.
--
-- Run it in the standard Lua interpreter in interactive mode, and you will
-- have a Lua shell with all the glmath facilities:
--
-- $ lua -i interactive.lua
-- > 
--
--

glmath = require("moonglmath")

-- globalize all glmath functions:
for k, v in pairs(glmath) do
   if(k~="type") then _ENV[k] = v end
end

-- globalize all math functions (except for type()):
for k, v in pairs(math) do
   if(k~="type") then _ENV[k] = v end
end
