-- lua_stubs/audio_api.lua

---@class Oscillator
---@field freq lightuserdata
---@field amp lightuserdata
Oscillator = {}

---@class LFO
---@field freq lightuserdata
---@field amp lightuserdata
---@field base number
LFO = {}

---@param target lightuserdata
function LFO:connect(target) end
