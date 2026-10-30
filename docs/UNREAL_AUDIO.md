# Unreal audio — 0.6.1

Marty can bark with B or LB/L1 during play. A 1.2-second cooldown prevents repeated button presses from stacking barks. Barking is free, briefly makes a rested Marty excited, and is disabled in menus or during a timed action.

Home/title music and backyard music use the project's retained original 40-second loops. Outdoor play selects Backyard; returning home or entering menus selects Home. One persistent music component changes tracks, and a muted track can resume when its volume is raised. Pause keeps music/UI sound available while pet-action logic stops.

The Sound settings menu independently adjusts saved music and effects volumes. Effects at zero suppress new effect playback. Walking/hurrying produces soft paw steps only while grounded and moving. Jump and landing have separate cues. Digging/burying uses scratching, meals use chewing, and toy pickup/drop uses a short playful squeak. Existing discovery, cat, cozy and menu sounds are retained.

## Sources and extension

The new Bark, Dig, Crunch and Toy WAV files are original deterministic synthesis in Unreal/UE5MovementLab/AudioSources. Regenerate them with Scripts/generate_pet_audio.py using standard Python. Bark is a stylized small-dog yap, not a recording of Marty. No third-party samples or dependencies are used. Existing Unity audio/source files remain unchanged; Step, Jump, Land and Backyard are imported copies of retained resources.

IMPORT_AUDIO.ps1 preserves existing Unreal assets and imports missing sounds. Deliberately reimport an edited WAV through Unreal if replacing an existing asset; running the script alone does not overwrite authored assets. Keep replacement samples appropriately licensed. The session owns music/effect volume and playback; DogLife emits contextual action cues.

## Verification

TEST_AUDIO.ps1 runs the real game map with the audio device enabled. It sends synthetic B/LB input, checks the cooldown and pause, verifies loaded sounds and music selection, and records only the game's master submix. Barks.wav and Music.wav must contain nonzero output without clipping; Muted.wav must be effectively silent. Audio recordings use an isolated acceptance save slot and remain under ignored Saved folders. This does not record microphones or other applications. Physical controllers and subjective speaker/headphone quality require separate listening checks.

After compiling and importing, run TEST_AUDIO.ps1. Use -GameExe to test the standalone executable. FINALIZE_PACKAGE.ps1 requires fresh audio assertions and waveform evidence as well as the existing gameplay reports before the launcher selects 0.6.1. Previous validated packages remain available.

The final standalone audio run passed ten assertions. Recorded bark/music had nonzero output and no clipping; the mute recording had RMS/peak exactly zero. See [packaged audio checks](evidence/ue5-061-audio-packaged.json) and [waveform measurements](evidence/ue5-061-audio-waveforms.json). Offscreen acceptance overrides background-window mute and keeps silent submixes rendering for capture; normal play retains Unreal's focus/silence policy. These are measured output checks, not subjective listening approval.
