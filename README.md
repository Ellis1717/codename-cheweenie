# Codename .... Cheweenie 🐾

A cozy Unity adventure starring Marty the Chiweenie, Lakitu (Keetz / Keeterman), and Rio (Sis / Little Miss / Miss Priss).

## Quick start

**Ready-to-play local Windows build:** close an older copy, then double-click **Play Cheweenie** on the desktop or `PLAY.cmd` in this folder. The current package is `builds/Current/Cheweenie.exe`; Unity can stay closed. Extract the entire `builds/Codename-Cheweenie-Family-0.5.0.zip` to play on another Windows PC.

Choose **Start adventure** or **Continue adventure**. WASD moves, Space jumps, E greets/sniffs, Shift sprints, C snuggles and Escape opens Settings. For the personalized neighbourhood, leave through the south garden gate and press M for the map; visit the park and school, then return Home. Settings → **Return Marty home** brings you back while keeping progress. [Editable assumptions and decisions](docs/ASSUMPTIONS.md) records provisional choices you can change later.

To develop or play in the editor:

If an older checkout showed a `TagManager.asset` parser error, use the latest source, clear historical Console entries and press Play again. Unnamed layers now use explicit empty strings; the settings validator also catches bare layer markers. The editor's Game view is empty before Play because the world is built at runtime.

1. Open this folder in **Unity 6.3 LTS** with an active editor license and resolve the manifest packages.
2. Use **Input System Package (New)** for Active Input Handling and restart Unity if prompted. Run **Cheweenie > Validate project configuration**. The manifest includes Input System 1.17.0 and Unity Pipeline 0.9.1-exp.1 for local CLI validation.
3. Press Play in an empty or default scene. Bootstrap temporarily disables existing scene cameras, audio listeners and directional lights, restoring them when its world is destroyed. The world builds behind a branded title screen. Choose **Start adventure** (or **Continue adventure** for existing progress).
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

Named Input System controls support Xbox and PlayStation devices. Settings provide label presets and button rebinding, saved between sessions. The camera follows automatically; settings change distance. Menu navigation uses arrows/stick and Enter/controller face buttons, with Escape always available.

Music plays on the title and in the world, fading to a brighter loop outdoors. Settings offers separate **Music** and **Sound effects** volumes; use Left/Right or horizontal stick input to adjust. Stop and restart Play after updating scripts, and ensure Game view **Mute Audio** is off. See [audio assets and controls](docs/AUDIO.md).

## Implemented foundations

The **neighbourhood extension** adds an open east garden gate, a walking street, three houses, a mailbox, flower garden, park/pond, benches, signs and a live map. Explore immediately or collect three sniff stamps and check in at the welcome-home sign. Existing progress remains compatible. See [neighbourhood route](docs/NEIGHBOURHOOD.md). The default layout is fictional. An ignored private map resource can replace it with real local streets, a park and a school stop; see [private local maps](docs/LOCAL_MAPS.md).

- **M1:** pure quest progression, strict Cozy completion, ground-probed jumping, explicit input axes, EditMode tests and a PlayMode smoke test.
- **M2:** configurable canine movement, procedural four-paw animation, optional Animator hooks, and an import-ready custom dog prefab pipeline.
- **M3:** modular schematic room definitions, upstairs loft, railed ramp, editable dimensions and optional photo panels.
- **M4:** distinct cat state machines, friendship, companion quests, obstacle checks and Lakitu following.
- **M5:** blanket resting, Cheween in Between cuddle placeholders, original synthesized chime, optional sparkles, versioned save data, pause/settings menu and keyboard/controller button rebinding.
- **M6:** a schematic backyard with stepping stones, a raised lookout, a secret hedge passage and a ribbon-recovery mission. It unlocks after the home quest; explore three landmarks in any order, find Rio's ribbon and bring it home. Companion dialogue, marker states, sight-line interaction checks and saved progress support the new loop. See [backyard route and verification](docs/BACKYARD.md).

Lakitu and Rio now use the supplied photos for their distinct black-and-white coat patterns in both the cover and 3D visuals. These remain stylized approximations. House geometry, Marty likeness and people remain placeholders. No exact floor plan or user-specific house measurements are assumed. User art, rigging, final lighting and expanded world content remain future work.

The title and settings share original cover concept artwork, a live wordmark, a paw mark, cream text and amber accents. The title freezes gameplay until Start/Continue; title settings return to the paused title. Mouse, keyboard and controller menu navigation are supported in source. See [branding and artwork provenance](docs/BRANDING.md).

## Verification status
On **2026-10-29**, **56 native Unity EditMode tests and seven PlayMode tests passed** in an isolated project copy. They cover imported audio/materials, title/pause flow, default-scene camera/listener ownership, home recovery, floor settling, home/backyard/neighbourhood quests, old-save compatibility, resets, virtual keyboard/Xbox/PlayStation controls and audio playback/crossfades. Runtime, editor and test sources compile with zero warnings/errors. **145 standalone checks pass**, including 33 audio checks.

The Windows 0.5.0 build passes all **38 full-adventure checks**: both Cozy moves, ribbon delivery, the complete local park/school patrol, return indoors and the recovery button. The 375.47-second run had zero runtime errors and 59.93 average FPS at 1280×720 on this machine. Separate launches pass **10 save/restart checks**, restoring all missions, friendship, bindings, volumes and camera settings. Both 1280×720 and 1920×1080 pass **18 menu/input/visual checks** each. Run `PLAY.cmd`, `builds/Current/Cheweenie.exe` or extract the family ZIP. See [validation evidence](docs/VALIDATION.md). Tests use virtual devices; physical controller hardware and speaker mix quality still need a human check. The prototype retains placeholder Marty/house/people art and stylized photo-guided cats.

See [validation and Windows build instructions](docs/VALIDATION.md), [Marty art import](docs/ART_IMPORT.md), [editable headquarters](docs/HEADQUARTERS.md), [architecture](docs/ARCHITECTURE.md), and [development backlog](docs/CODEX_HANDOFF.md). Read AGENTS.md before changing the project.
