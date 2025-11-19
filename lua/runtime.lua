-- Volana Lua runtime helpers for the high-level DSL.

local registry = debug.getregistry()
local PARAM_METATABLE = registry and registry["Param"]

local function is_param(value)
	return type(value) == "userdata" and PARAM_METATABLE and getmetatable(value) == PARAM_METATABLE
end

local ParamProxy = {}
ParamProxy.__index = ParamProxy

local function set_audio_output(node, enabled)
	if type(node) ~= "userdata" then
		return
	end
	local setter = node and node.set_audio_out
	if type(setter) == "function" then
		setter(node, enabled)
	end
end

local function set_processing(node, enabled)
	if type(node) ~= "userdata" then
		return
	end
	local setter = node and node.set_processing
	if type(setter) == "function" then
		setter(node, enabled)
	end
end

function ParamProxy.new(builder, param)
	return setmetatable({ _builder = builder, _param = param }, ParamProxy)
end

function ParamProxy:_forward(method, ...)
	local fn = self._param[method]
	if type(fn) ~= "function" then
		error(string.format("Param does not support '%s'", method), 2)
	end
	fn(self._param, ...)
	return self._builder
end

function ParamProxy:c(...)
	return self:_forward("c", ...)
end

ParamProxy.chain = ParamProxy.c

function ParamProxy:m(...)
	return self:_forward("m", ...)
end

ParamProxy.mod = ParamProxy.m

function ParamProxy:e(...)
	return self:_forward("e", ...)
end

ParamProxy.effect = ParamProxy.e

function ParamProxy:raw()
	return self._param
end

local SoundBuilder = {}

local function add_audio_node(self, node)
	if type(node) ~= "userdata" then
		return
	end

	local seen = rawget(self, "_audioSeen")
	if seen[node] then
		return
	end

	seen[node] = true
	table.insert(self._audioNodes, node)

	if rawget(self, "_playing") then
		set_processing(node, true)
	else
		set_processing(node, false)
		set_audio_output(node, false)
	end
end

local function resolve_param(self, key)
	if not PARAM_METATABLE then
		return nil
	end

	local search = {}
	local focus = rawget(self, "_focus")
	if focus then
		table.insert(search, focus)
	end
	local source = rawget(self, "_source")
	if source then
		table.insert(search, source)
	end

	for _, node in ipairs(search) do
		if node then
			local ok, value = pcall(function()
				return node[key]
			end)

			if ok and is_param(value) then
				return value
			end
		end
	end

	return nil
end

local soundMethods = {}

function soundMethods.e(self, effect)
	if effect == nil then
		error("sound:e() expects an effect node", 2)
	end
	local head = rawget(self, "_audioHead")
	if not head then
		error("sound has no source to attach effects", 2)
	end

	if rawget(self, "_playing") then
		set_audio_output(head, false)
	end

	head:pipe(effect)
	rawset(self, "_audioHead", effect)
	rawset(self, "_focus", effect)

	add_audio_node(self, effect)

	if rawget(self, "_playing") then
		set_audio_output(effect, true)
	end
	return self
end

soundMethods.effect = soundMethods.e

function soundMethods.play(self)
	local head = rawget(self, "_audioHead")
	if not head then
		error("sound needs a source before play()", 2)
	end

	set_audio_output(head, true)
	for _, node in ipairs(self._audioNodes) do
		set_processing(node, true)
	end
	rawset(self, "_playing", true)
	return self
end

function soundMethods.cue(self, symbol)
	self._cue = symbol
	return self
end

function SoundBuilder.__index(self, key)
	local method = soundMethods[key]
	if method then
		return function(...)
			return method(self, ...)
		end
	end

	local param = resolve_param(self, key)
	if param then
		return ParamProxy.new(self, param)
	end

	error(string.format("Unknown sound field '%s'", tostring(key)), 2)
end

local function make_builder(oscillator)
	if oscillator == nil then
		error("sound() expects an oscillator", 2)
	end

	return setmetatable({
		_source = oscillator,
		_audioHead = oscillator,
		_focus = nil,
		_cue = nil,
		_playing = false,
		_audioNodes = {},
		_audioSeen = {},
	}, SoundBuilder)
end

function sound(oscillator)
	local builder = make_builder(oscillator)
	add_audio_node(builder, oscillator)
	return builder
end

function osc(...)
	return Oscillator(...)
end

function lfo(...)
	return LFO(...)
end

function filter(...)
	return Filter(...)
end

PI = math.pi
TAU = math.pi * 2

return {
	sound = sound,
	osc = osc,
	lfo = lfo,
	filter = filter,
}
