-- create an oscillator
osc1 = Oscillator({ freq = 440, amp = 0.1 }) --, type = "sine" })
osc2 = Oscillator(0.2, 220)

lfo1 = LFO(440, 40, 3)
lfo1:connect(osc1.freq)

lfo2 = LFO(220, 40, 1)
lfo2:connect(osc2.freq)

lfo3 = LFO(3, 3, 1)
lfo3:connect(lfo1.freq)

lfo4 = LFO(440, 20, 20)
lfo4:connect(lfo1.base)

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
