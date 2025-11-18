-- create an oscillator
-- Osc1 = Oscillator({ freq = 220, amp = 0.1, type = Saw }) --, type = "sine" })
-- Osc2 = Oscillator(0.1, 220)
--
-- Lfo1 = LFO(220, 30, 2)
-- Lfo1:mod(Osc2, "freq")
--
-- Filter = Filter({ q = 2 })
-- Osc1:pipe(Filter)
--
-- Lfo2 = LFO(1000, 700, 0.25)
-- Lfo2:mod(Filter, "cutoff")

-- Osc1 = Oscillator(0.1, 220)
-- Osc2 = Oscillator(0.1, 440)
-- Osc3 = Oscillator(0.1, 660)
-- Lfo1 = LFO(440, 20, 0.5)
-- Adsr1 = ADSR(0.1, 0.2, 0.3, 0.4)
-- Fil1 = Filter(1000, 2)
-- Del1 = Delay(0.25, 0.5) -- rate, feedback
-- Lfo1:mod(Osc2, "freq")
-- Adsr1:mod(Fil1, "cutoff")
-- Adsr1:mod({Osc1, Osc2, Osc3}, "amp")
-- V1 = ({Osc1, Osc2, Osc3}..Fil1..Del1):voice("o")
--
-- -- create oscillators, apply control nodes, create voice with piped effects
-- -- adsr does nothing wihtout pattern
--
-- Osc4 = Oscillator(0.1, 440, Saw)
-- Lfo2 = LFO(440, 110, 10)
-- Rand1 = RandInt(4, 16) -- timing, bounds
-- Fil2 = Filter(500, 1)
-- Lfo2:mod(Osc4, "freq")
-- Rand1:mod(Lfo2, "freq")
-- V2 = (Osc4..Fil2):voice(nil) -- no pattern symbol, continuous play
--
-- Pattern("o o o o [o o]*4"):play() -- or...
-- p = Pattern("o o o o [o o]*4")
-- Rand1:trigger(p)
-- p:play()
-- V2:play()
--
-- osc(0.1, 440, Sine)
-- 	.lfo(440, 110, 2, "freq")
-- 	.filter(1000, 2)
-- 	.lfo(1000, 700, 0.25, "cutoff")
-- 	.adsr(0.1, 0.1, 0.1, 0.1, "amp")
-- 	.delay(0.5, 1)
-- 	.play()
--
-- osc(0.1, 440, Sine)
-- 	:lfo(440, 110, 2, "freq")
-- 	:lfo(0.1, 0.1, 1, "amp")
-- 	:adsr(0.1, 0.1, 0.1, 0.1, "amp")
-- 	.filter(1000, 2)
-- 	:lfo(1000, 700, 0.25, "cutoff")
-- 	:adsr(0.1, 0.1, 0.1, 0.1, "amp")
-- 	.delay(0.5, 1)
-- 	.play("x") -- .play() to play cont., .play("x") to assing pattern name x and wait for pattern
--
-- a = adsr(0.1, 0.1, 0.1, 0.1)
--
-- osc(0.1, 440, Sine)
-- 	:lfo(440, 110, 2, "freq")
-- 	:lfo(0.1, 0.1, 1, "amp")
-- 	:adsr(a, "amp")
-- 	.filter(1000, 2)
-- 	:lfo(2, 1, 0.25, "q")
-- 	:adsr(a, "cutoff")
-- 	.delay(0.5, 1)
-- 	.play("x")

-- osc(0.1, 440, Sine).freq
-- 	:chain(lfo(440, 110, 2).amp:lfo(0.1, 0.1, 1)).amp
-- 	:mod(a)
-- 	.filter(1000, 2).q
-- 	:lfo(2, 1, 0.25).cutoff
-- 	:mod(a)
-- 	.delay(0.5, 1)
-- 	.play("x")
--
-- o = osc(0.1, 440, Sine)
-- a = adsr(0.1, 0.1, 0.1, 0.1)
-- l1 = lfo(440, 110, 2)
-- l2 = lfo(0.1, 0.1, 1)
-- l3 = lfo(2, 1, 0.25)
-- f = filter(1000, 2)
-- d = delay(0.5, 1)
--
-- sound(o)
--   .freq:chain(l1
--     .amp:mod(l2))
--   .amp:mod(a)
--   .effect(f)
--   .q:effect(l3)
--   .cutoff:mod(a)
--   .effect(d)
--   .cue("x")
--
-- sound(o)
--   .freq:c(l1
--     .amp:m(l2))
--   .amp:m(a)
--   .e(f)
--   .q:m(l3)
--   .cutoff:m(a)
--   .e(d)
--   .cue("x")
--
-- pattern("x x x x").play()

o = osc(0.1, 440, Saw)
x = osc(0.1, 220, Sine)
l1 = lfo(440, 220, 2)
l2 = lfo(2, 2, 1)
l3 = lfo(1000, 700, 0.25)
f = filter(1000, 2)

sound(x).play()
sound(o).freq:c(l1.freq:m(l2)).e(f).cutoff:m(l3).play()

-- sound(o).freq:chain(l1.freq:mod(l2)).effect(f).cutoff:mod(l3).play()

-- sound(osc(0.1, 440, Saw)).freq
-- 	:chain(lfo(440, 220, 2).freq:mod(lfo(2, 2, 1)))
-- 	.effect(filter(1000, 2)).cutoff
-- 	:mod(lfo(1000, 700, 0.25))
-- 	.play()
