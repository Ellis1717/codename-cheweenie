# Codename .... Cheweenie 🐾

A cozy Unity adventure starring Marty the Chiweenie, Lakitu (Keetz / Keeterman), and Rio (Sis / Little Miss / Miss Priss).

## Quick start
1. Open this folder in **Unity 6.3 LTS** with an active editor license and resolve the manifest packages.
2. Use **Input Manager (Old)** or **Both** for Active Input Handling. Run **Cheweenie > Validate project configuration**.
3. Press Play in an empty scene. The world builds behind a branded title screen. Choose **Start adventure** (or **Continue adventure** for existing progress).
4. Open **Settings** from the title to edit volume, camera, effects and bindings. **How to play** explains controls. During gameplay, Settings also offers **Return to title**.
5. Find Lakitu, find Rio, sniff the backyard, then use **Cheeween Cozy** at the blanket fort. **New adventure** confirms before clearing existing progress and preserves settings.

| Action | Keyboard | Standard gamepad |
| --- | --- | --- |
| Move | WASD / arrows | Left stick |
| Jump | Space | South / A / Cross |
| Sniff / greet | E | West / X / Square |
| Sprint | Left Shift | East / B / Circle |
| Cheeween Cozy | C | North / Y / Triangle |
| Cheween in Between | B | Left shoulder |
| Settings | Escape | Start / Options |

Controller mappings vary by driver. Settings provide presets and button rebinding, saved between sessions. The camera follows automatically; settings change distance. Menu navigation uses arrows/stick and Enter/raw button 0, with Escape always available.

Music plays on the title and in the world, fading to a brighter loop outdoors. Settings offers separate **Music** and **Sound effects** volumes; use Left/Right or horizontal stick input to adjust. Stop and restart Play after updating scripts, and ensure Game view **Mute Audio** is off. See [audio assets and controls](docs/AUDIO.md).

## Implemented foundations
- **M1:** pure quest progression, strict Cozy completion, ground-probed jumping, explicit input axes, EditMode tests and a PlayMode smoke test.
- **M2:** configurable canine movement, procedural four-paw animation, optional Animator hooks, and an import-ready custom dog prefab pipeline.
- **M3:** modular schematic room definitions, upstairs loft, railed ramp, editable dimensions and optional photo panels.
- **M4:** distinct cat state machines, friendship, companion quests, obstacle checks and Lakitu following.
- **M5:** blanket resting, Cheween in Between cuddle placeholders, original synthesized chime, optional sparkles, versioned save data, pause/settings menu and keyboard/controller button rebinding.
- **M6:** a schematic backyard with stepping stones, a raised lookout, a secret hedge passage and a ribbon-recovery mission. It unlocks after the home quest; explore three landmarks in any order, find Rio's ribbon and bring it home. Companion dialogue, marker states, sight-line interaction checks and saved progress support the new loop. See [backyard route and verification](docs/BACKYARD.md).

House geometry, character likenesses and people are placeholders. No exact floor plan or user-specific house measurements are assumed. User art, rigging, final lighting and expanded world content remain future work.

The title and settings share original cover concept artwork, a live wordmark, a paw mark, cream text and amber accents. The title freezes gameplay until Start/Continue; title settings return to the paused title. Mouse, keyboard and controller menu navigation are supported in source. See [branding and artwork provenance](docs/BRANDING.md).

## Verification status
On **2026-10-28**, **28 native Unity EditMode tests and two PlayMode tests passed** in an isolated project copy. They cover imported audio, title/pause flow, floor settling, home/backyard quest interactions, resets and audio playback/crossfades. Runtime, editor and test sources also compile with zero warnings/errors. **145 standalone checks pass**, including 33 audio checks.

Unity licensing now permits import and testing; earlier exit-198 attempts are historical. Speaker output/mix quality, rendered visuals, physical keyboard/gamepad play, full manual traversal and Windows builds still need verification. The prototype retains placeholder character/house models.

See [validation and Windows build instructions](docs/VALIDATION.md), [Marty art import](docs/ART_IMPORT.md), [editable headquarters](docs/HEADQUARTERS.md), [architecture](docs/ARCHITECTURE.md), and [development backlog](docs/CODEX_HANDOFF.md). Read AGENTS.md before changing the project.
