-- create an oscillator
local osc1 = Oscillator({ freq = 220, amp = 0.1, type = Saw }) --, type = "sine" })
local osc2 = Oscillator(0.1, 220)

local lfo1 = LFO(220, 30, 2)
lfo1:mod(osc2, "freq")

local filter = Filter({ q = 2 })
osc1:pipe(filter)

local lfo2 = LFO(1000, 700, 0.25)
lfo2:mod(filter, "cutoff")

-- local osc1 = Oscillator({ freq = 440, amp = 0.1, type = Saw }) --, type = "sine" })
-- local osc2 = Oscillator(0.2, 220)
--
-- local lfo1 = LFO(440, 40, 3)
-- lfo1:mod(osc1, "freq")
--
-- local lfo2 = LFO(220, 40, 1)
-- lfo2:mod(osc2, "freq")
--
-- local lfo3 = LFO(3, 3, 1)
-- lfo3:mod(lfo1, "freq")
--
-- local lfo4 = LFO(440, 20, 20)
-- lfo4:mod(lfo1, "base")
--
-- lfo4.freq = 10
--
-- -- filtering
-- local filter = Filter({ cutoff = 1000, q = 6 })
-- osc1:pipe(filter)
--
-- local lfo5 = LFO(500, 400, 1)
-- lfo5:mod(filter, "cutoff")

-- complete usage example
-- -- create a voice
-- arp = Voice({ osc = osc1, pattern = { "C3", "E3", "G3", "B3" } })
--
-- -- add voice to engine
-- add_voice(arp)
--
-- -- live modulation
-- osc1.freq = 440
-- arp:set_adsr(0.01, 0.2, 0.7, 0.3)
--
-- ---------
-- osc1.freq:connect(lfo1)
-- filter.cutoff:connect(env1 * lfo2)
-- macro:connect(lfo1 + env1)
