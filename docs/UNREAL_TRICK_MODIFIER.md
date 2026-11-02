# Codename .... Cheweenie — direct trick controls

Hold **L1 / LB**, then press a face button to perform a trick immediately. The game stays in play; a small corner guide replaces the ordinary lower control hints while the shoulder button is held.

| Action | PlayStation, while holding L1 | Xbox, while holding LB | Keyboard, while holding Left Ctrl |
|---|---|---|---|
| Lie down / get back up | Cross | A | 1 |
| Roll over | Circle | B | 2 |
| Shake a paw | Square | X | 3 |
| Bork | Triangle | Y | 4 |

Movement and the camera remain active while holding the modifier. Roll and shake use the existing timed animations and their normal movement locks. Release the modifier to return to the usual controls. Movement also gets Marty up; center the stick to stay lying down. **C / unmodified Circle-B** still opens the trick menu as an alternative. Keyboard **B** still barks immediately; a quick **L1/LB tap** (under 0.3 seconds) barks on release. A longer hold or a used trick combination never adds a release bark. Tapping Left Ctrl does not bark.

The face buttons are consumed by the modifier layer, so a trick does not also jump, hurry, interact or open a menu. Adding the modifier to a face button that is already held cancels that held-button hurry request but waits for a fresh face press before performing a trick. Releasing the modifier while a face button remains held does not start its ordinary action mid-hold. Pause/menu transitions disarm the layer until the modifier is released; menu confirmation/back continue to work normally.

These shortcuts reuse Marty's existing core-adventure actions. **Controls → Pet feature switches → Dog tricks / Barking** retain their independent switches; disabled or busy actions consume the combination without falling through to another action. **Screen overlays → Direct trick guide** controls the corner guide independently of ordinary control hints. The LAN room retains its existing voice/jump controls; this change does not add replicated trick animation for the cats.

No save-schema, model, texture or Unity change is needed. Earlier packages and the 0.12.0 core build remain available through Archived versions.

Verification commands: `BUILD.ps1`, `TEST_TRICK_MODIFIER.ps1` (optionally `-Render`), `TEST_TRICKS.ps1`, `TEST_AUDIO.ps1` and the standard movement/menu/gameplay regressions. The dedicated modifier suite uses synthetic keyboard/gamepad input in the real map and an isolated save. Rendered checks capture the guide and three poses. Audio acceptance separately records and checks bark/music/mute waveforms. Actual release results are recorded in README and evidence after verification; physical controller testing must be reported separately.


Final 0.12.1 verification: 635 assertion executions and three audio waveform checks passed after Editor/Game builds and native staging with unchanged cooked content. Four new captures were reviewed and 43 payload hashes passed; final runtime errors/material fallbacks/VSM overflows were zero. The 565-check standalone gate includes 32 direct-modifier checks and the existing 34-check trick suite. Physical controller/two-PC checks remain unverified. [Actual evidence](evidence/ue5-trick-modifier-build.json).