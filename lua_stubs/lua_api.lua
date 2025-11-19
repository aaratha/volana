-- Oscillator types
Sine = 0
Saw = 1
InvSaw = 2
Square = 3
Triangle = 4

---@type number
PI = math.pi

---@type number
TAU = math.pi * 2

---@class Oscillator
---@field freq lightuserdata
---@field amp lightuserdata
---@field type integer One of Sine, Saw, InvSaw, Square, Triangle
---@field audioOut boolean
Oscillator = {}

---@class LFO
---@field base lightuserdata
---@field amp lightuserdata
---@field freq lightuserdata
---@field shift lightuserdata
---@field type integer One of Sine, Saw, InvSaw, Square, Triangle
LFO = {}

---@class Filter
---@field cutoff lightuserdata
---@field q lightuserdata
---@field inputs lightuserdata[]
---@field audioOut boolean
Filter = {}

-- Methods

---Connect this LFO to a node parameter
---@param node table Oscillator or LFO node
---@param param string Name of the parameter to modulate ("freq", "amp", "base", etc.)
function LFO:mod(node, param) end

---Pipe this oscillator into a filter
---@param filter Filter
function Oscillator:pipe(filter) end
