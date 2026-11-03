# Natural pet recordings — CC0 1.0

These files are real animal recordings, used as stand-in voices for the game pets.
They are not recordings of Marty, Lakitu or Rio. Retrieved November 3, 2026.
All three source pages explicitly designate the recordings Creative Commons 0.

- `SmallDog.mp3`: **Smal_Dog_Barking.wav**, ivolipa, January 4, 2016.
  [Source and CC0 declaration](https://freesound.org/people/ivolipa/sounds/332457/).
  `Bark01.wav`–`Bark04.wav` are four separate cuts from this recording.
- `Lakitu.mp3`: **Cat Meow**, qubodup, June 22, 2025; derived from
  **Cat Call (Meow)** by Jofae (CC0).
  [Source and CC0 declaration](https://freesound.org/people/qubodup/sounds/813119/).
- `Rio.mp3`: **Cat Meowing**, qubodup, June 22, 2025; derived from
  **cat meow II** by tuberatanka (CC0).
  [Source and CC0 declaration](https://freesound.org/people/qubodup/sounds/813113/).

[CC0 1.0 Universal legal terms](https://creativecommons.org/publicdomain/zero/1.0/legalcode).
Commercial use, modifications and redistribution are allowed. Credits are retained
for provenance, though attribution is not a CC0 condition.

The retained MP3s are Freesound's public HQ previews, not the lossless originals.
Conversion to WAV does not recover MP3 detail. Processing: mono downmix, individual
cuts, gentle 70 Hz high-pass, 8 ms edge fades and peak normalization to 0.63
(approximately −4 dBFS), with no pitch shifting or synthesized vocal layers.
`manifest.json` records exact source/download URLs, cuts and SHA-256 hashes.

Rebuild offline with `Scripts/build_natural_pet_audio.py` using Python 3,
NumPy and SoundFile (authoring tools only). Import with `IMPORT_NATURAL_AUDIO.ps1`.
The complete shipped pack is self-contained and needs no online service or paid
dependency. The earlier synthesized pack remains in its original locations.
