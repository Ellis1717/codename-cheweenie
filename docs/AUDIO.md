# Original music and sound

The game bundles two original forty-second, 96 BPM instrumental loops: soft keys/pads for the title and home, and brighter plucks for backyard exploration. They share timing/harmony and start on the same audio DSP clock. An equal-power crossfade follows the player's outdoor position; returning to title selects the home theme. Music continues during title/settings pause, with quieter levels in settings and while Cozy.

Nine original short cues cover menu selection, paw steps, jumping, landing, Lakitu, Rio, Cozy, discovery and Between. Cat sounds are stylized synthesized chirps, not recordings or voice acting. Footsteps follow grounded travel distance and stay silent while airborne or stationary. Gameplay cues are suppressed on the paused title; menu feedback remains enabled.

## Controls and save migration

Settings has separate **Sound effects** and **Music** rows. Use Left/Right or horizontal stick input for 10% adjustments, or activate/click a row to cycle. Zero mutes that category. Effects changes preview a menu sound. Both gains persist and survive New Adventure.

Existing `volume` remains the effects gain. Additive fields `audioVersion` and `musicVolume` migrate older saves to 35% music, while preserving an existing effects mute by initially muting music too. Once migrated, zero music remains an intentional independent mute.

## Assets and provenance

All music, notes, synthesis and WAVs were created for this project; no commercial tracks, samples, streaming services or paid dependencies are used. Source composition/synthesis is in `OriginalAudio.cs`. PCM assets live under `Assets/Resources/Audio` and are included offline in builds.

| Assets | Duration | Format |
| --- | --- | --- |
| Home.wav, Backyard.wav | 40s each | Stereo, 22050 Hz, 16-bit PCM WAV |
| Menu, Step, Jump, Land, Lakitu, Rio, Cozy, Discovery, Between | 0.12–0.85s | Stereo, 22050 Hz, 16-bit PCM WAV |

Regenerate from the project root:

```powershell
dotnet run --project Validation/AudioAssets.csproj
```

The generator stages complete files before replacing WAVs, and rejects non-finite, clipped or silent PCM. `OriginalAudioImport` preserves stereo/sample rate, imports lossless PCM and preloads audio. Eleven files total about 7.6 MB. `GameAudio` owns music and effect sources; `CozyFeedback` keeps visual effects and routes the appropriate sound cue to that bus. Inspector music overrides are supported for future custom tracks; use matching loop lengths/tempo when replacing both for synchronized transitions.

## Verification

28 native Unity EditMode and two PlayMode tests pass in an isolated project copy. The audio PlayMode test verifies that music sample position advances while gameplay is paused, menu audio plays, music/effects mute independently, and the backyard/home transition changes relative source gains. The 145-check standalone suite includes 33 audio checks. These tests do not audition speaker output or judge the mix; listen in the real Game view with Mute Audio off.

The installed Unity CLI can see the editor but the project has no Pipeline package for live control. Tests used the native editor batch runner instead. CLI beta.14 is available over installed beta.13; no CLI update or package installation was performed for this feature.
