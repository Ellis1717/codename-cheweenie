"""Original deterministic synthesized effects; no recordings or third-party samples.

Run with standard Python 3. Sources are retained beside this script's project.
The bark is a stylized small-dog yap, not a recording of Marty.
"""
import array
import math
import pathlib
import random
import wave

ROOT = pathlib.Path(__file__).resolve().parents[1] / 'AudioSources'
RATE = 24000


def render(name, duration, sample):
    rng = random.Random(1717)
    pcm = array.array('h')
    for i in range(round(duration * RATE)):
        t = i / RATE
        # Short edge fades avoid clicks; reserve headroom for music and other effects.
        edge = min(1, t / .008, (duration - t) / .02)
        value = max(-.68, min(.68, sample(t, rng))) * max(0, edge)
        pcm.append(round(value * 32767))
    ROOT.mkdir(exist_ok=True)
    with wave.open(str(ROOT / (name + '.wav')), 'wb') as out:
        out.setparams((1, 2, RATE, 0, 'NONE', 'not compressed'))
        out.writeframes(pcm.tobytes())


def bark(t, rng):
    # Two breathy voiced pulses with falling pitch and formant-like harmonics.
    value = 0
    for start, length, pitch in ((.02, .20, 245), (.29, .24, 225)):
        u = t - start
        if 0 <= u < length:
            env = (1 - math.exp(-u * 180)) * math.exp(-u * 18)
            phase = 2 * math.pi * (pitch * u - 180 * u * u)
            voice = sum(math.sin(phase * h) * amp for h, amp in ((1,.16),(2,.22),(3,.35),(4,.18),(6,.09)))
            value += env * (voice + rng.uniform(-.20, .20))
    return value * 1.2


def scratch(t, rng):
    u = t % .25
    return rng.uniform(-1, 1) * .30 * math.exp(-u * 32) * (1 + math.sin(t * 2900)) / 2


def crunch(t, rng):
    u = t % .18
    return (rng.uniform(-1, 1) * .33 + .10 * math.sin(t * 4200)) * math.exp(-u * 55)


if __name__ == '__main__':
    render('Bark', .62, bark)
    render('Dig', .85, scratch)
    render('Crunch', 1.15, crunch)
    render('Toy', .18, lambda t, r: .32 * math.sin(2 * math.pi * (700*t-900*t*t)) * math.exp(-t*24))
