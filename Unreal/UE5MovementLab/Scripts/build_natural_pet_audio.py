"""Prepare retained CC0 MP3 recordings. Offline; authoring requires numpy/soundfile.

The shipped game only needs imported Unreal assets. No Python runtime dependency.
Original synthesized effects and source MP3s are never overwritten.
"""
import hashlib
import json
from pathlib import Path
import numpy as np
import soundfile as sf

ROOT = Path(__file__).resolve().parents[1] / 'AudioSources' / 'NaturalPets'
SOURCES = {
    'SmallDog': ('ivolipa', 'https://freesound.org/people/ivolipa/sounds/332457/',
                 'https://cdn.freesound.org/previews/332/332457_3474310-hq.mp3'),
    'Lakitu': ('qubodup; original recording Jofae', 'https://freesound.org/people/qubodup/sounds/813119/',
               'https://cdn.freesound.org/previews/813/813119_71257-hq.mp3'),
    'Rio': ('qubodup; original recording tuberatanka', 'https://freesound.org/people/qubodup/sounds/813113/',
            'https://cdn.freesound.org/previews/813/813113_71257-hq.mp3'),
}
CUTS = {'Bark01': ('SmallDog', .68, 1.47), 'Bark02': ('SmallDog', 3.19, 4.00),
        'Bark03': ('SmallDog', 6.04, 6.92), 'Bark04': ('SmallDog', 10.40, 11.25),
        'Lakitu': ('Lakitu', 0, .456), 'Rio': ('Rio', 0, .600)}


def build():
    report = {'license': 'CC0-1.0', 'license_url': 'https://creativecommons.org/publicdomain/zero/1.0/',
              'provenance': 'Real animal recordings; Freesound HQ MP3 previews, not lossless originals or recordings of the game pets.',
              'sources': {}, 'clips': {}}
    for name, (author, page, media) in SOURCES.items():
        report['sources'][name] = dict(author=author, page=page, download=media,
            sha256=hashlib.sha256((ROOT / (name+'.mp3')).read_bytes()).hexdigest())
    for name, (source, start, end) in CUTS.items():
        data, rate = sf.read(ROOT / (source+'.mp3'), always_2d=True)
        data = data.mean(axis=1)[round(start*rate):round(end*rate)].copy()
        # Remove DC/low rumble with a gentle 70 Hz one-pole high-pass.
        alpha = np.exp(-2*np.pi*70/rate)
        previous_input = previous_output = 0.
        for i, value in enumerate(data):
            previous_output = alpha*(previous_output+value-previous_input)
            previous_input = value
            data[i] = previous_output
        fade = min(round(.008*rate), len(data)//2)
        data[:fade] *= np.linspace(0, 1, fade)
        data[-fade:] *= np.linspace(1, 0, fade)
        data *= .63 / max(np.max(np.abs(data)), .001)
        sf.write(ROOT/(name+'.wav'), data, rate, subtype='PCM_16')
        assert .1 < len(data)/rate < 1.2 and np.max(np.abs(data)) < .64
        report['clips'][name] = dict(source=source, start=start, end=end, rate=rate,
            seconds=len(data)/rate, peak=float(np.max(np.abs(data))), rms=float(np.sqrt(np.mean(data**2))),
            sha256=hashlib.sha256((ROOT/(name+'.wav')).read_bytes()).hexdigest())
    (ROOT/'manifest.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps(report['clips'], indent=2))


if __name__ == '__main__':
    build()
