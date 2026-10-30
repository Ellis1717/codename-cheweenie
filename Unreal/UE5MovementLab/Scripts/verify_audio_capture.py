"""Check actual Unreal submix WAV output; does not claim subjective listening QA."""
import array
import json
import math
import pathlib
import sys
import wave

root = pathlib.Path(sys.argv[1])
report = {}
for name in ('Barks', 'Music', 'Muted'):
    with wave.open(str(root / (name + '.wav')), 'rb') as wav:
        assert wav.getsampwidth() == 2, 'Expected 16-bit master capture'
        samples = array.array('h', wav.readframes(wav.getnframes()))
        duration = wav.getnframes() / wav.getframerate()
    assert samples and duration > .5, 'Capture is empty or too short'
    rms = math.sqrt(sum((v / 32768) ** 2 for v in samples) / len(samples))
    peak = max(abs(v) for v in samples) / 32768
    report[name] = dict(seconds=duration, rms=rms, peak=peak)
    assert rms < .0002 if name == 'Muted' else rms > .0002, (name, rms)
    assert peak < .999, 'Audio clipping in ' + name
(root / 'audio-waveforms.json').write_text(json.dumps(report, indent=2))
print(json.dumps(report, indent=2))
