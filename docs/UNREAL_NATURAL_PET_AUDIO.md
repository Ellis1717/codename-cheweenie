# Codename .... Cheweenie — recorded pet voices

Fresh PR #46–#49 integration verification supplies the formerly missing clean-checkout build and runtime coverage: native Editor compilation, 14 audio assertions and all three master-submix waveform checks passed, with exact-zero muted output. Movement passed 16/16 headlessly and 15/16 rendered; the existing RB/R1 recenter assertion remains unresolved, and its function is unchanged from pre-PR main. Physical controls and subjective listening remain unverified; this is not a new package. [Combined results](evidence/ue5-pr46-49-integration.json).

The Unreal development project now uses four distinct small-dog bark takes and
two real cat meows. Solo barking chooses randomly without repeating the previous
take; the shared LAN/memory-room pets use the same pack and selection rule.
These are stand-in recordings, not recordings of Marty, Lakitu or Rio.

All sources are CC0. The retained HQ MP3 previews, processed mono PCM WAVs,
source URLs, author credits, exact cuts and SHA-256 hashes live in
[`AudioSources/NaturalPets`](../Unreal/UE5MovementLab/AudioSources/NaturalPets/LICENSE.md).
They were trimmed, gently filtered at 70 Hz, faded at the edges and normalized
to approximately −4 dBFS. Converting the MP3 previews to WAV does not restore
lossless detail. No pitch shifting or synthesized layers were added to the voices.

Original synthesized Bark/Lakitu/Rio assets remain in place as missing-pack
fallbacks. Music, UI/action effects, saved settings, bark cooldowns, feature
switches and input mappings retain their existing behavior. Solo voices respect
the existing effects volume and mute. Shared-room voices retain their existing
spatial playback and room-volume behavior. Each listener chooses its own take;
the existing replicated event/cooldown protocol is unchanged.

The content-only comparison preview, Unity fallback and previous standalone
packages are retained. **The desktop Play Cheweenie package has not been rebuilt
for this change.** Use the native development project to play with the new pack.

## Rebuild and check

From `Unreal/UE5MovementLab`, run `BUILD.ps1`, `IMPORT_NATURAL_AUDIO.ps1`,
`TEST_AUDIO.ps1`, and `TEST.ps1 -Render` in that order. The checked-in Unreal
assets already contain the new recordings. `/Game/Adventure` is already an
always-cook directory, including this pack on future full cooks.

To edit/rebuild the WAVs offline, run `Scripts/build_natural_pet_audio.py` with
Python 3, NumPy and SoundFile installed. This is an optional authoring step;
game playback requires none of these tools or an online service. Reimport after
changing the WAVs. `generate_pet_audio.py` still only regenerates the older pack.

## Actual verification — November 3, 2026

Native Editor compilation passed on the original integration checkout with
Unreal **5.8.4**, MSVC **14.44.35229** and Windows SDK **10.0.22621.0**.
All six new SoundWave imports passed source-hash checks.

The rendered DX12 real-map audio suite passed **14 assertions**: recorded assets
load, keyboard B and synthetic LB tap-release bark, consecutive takes differ,
cooldown/pause suppress extra barking, companion/action cues spawn, travel
switches music, and effects mute suppresses playback. Three master-submix
waveform checks passed: bark RMS **0.014211**, peak **0.133026**; music RMS
**0.008970**, peak **0.039764**; mute RMS/peak **0**. The final audio runtime log
contained **zero Error/Fatal lines**.

A separate rendered movement regression passed **15 of 16** assertions;
**RB/R1 recenters camera** failed. A fresh headless rerun passed **all 16**,
including recenter. The rendered failure remains unresolved; this is not an
all-green rendered movement result. Neither the camera code nor that fixture was
changed for the audio pack. [Rendered report](evidence/ue5-natural-pet-movement-rendered.json),
[headless report](evidence/ue5-natural-pet-movement-headless.json),
[combined build record](evidence/ue5-natural-pet-audio-build.json).

Physical controllers, subjective listening, separate PCs, shared-room audible
playback, a clean PR-checkout build and a new packaged executable remain
unverified. Tests used synthetic input and the existing test-only save slot.
Unity source/assets/settings were not changed, so Unity checks were not rerun.
Existing fur, brick, UI and owner-resource changes were retained; the focused
audio branch is based on main and excludes those unrelated edits. Integration
results do not claim that its entire tree was tested independently.

Evidence: [imports](evidence/ue5-natural-pet-audio-import.json),
[runtime assertions](evidence/ue5-natural-pet-audio.json),
[recorded waveforms](evidence/ue5-natural-pet-audio-waveforms.json).
