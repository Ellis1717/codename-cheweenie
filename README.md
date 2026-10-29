# Codename .... Cheweenie 🐾

A cozy Unity adventure starring Marty the Chiweenie, Lakitu (Keetz / Keeterman), and Rio (Sis / Little Miss / Miss Priss).

**Family cameos (0.8.1):** sniff the backyard zone for **Fox Ellis, Chief Sniff Inspector: “Yep. Definitely grass.”** Get Cheeween Cozy at the blanket fort for **Leo Ellis, Blanket Boss: “Five more minutes!” Marty agrees.** Both messages stay visible for eight seconds and replay when you repeat the encounter after completing it. Cat friendship rewards still apply.

Marty now loads the original **Marty Explorer** mesh from your supplied character archive. His geometry and ten source colours are retained, with added leg, head, ear, tail and blink motion. Original OBJ/GLB, the Unity prefab, meshes, materials, importer and animation source are version controlled; a photo-guided alternative is retained too. See [Marty resources and editing](Assets/Art/Marty/README.md). The concept sheet has more detail than the supplied static mesh; it is an art target rather than a render of this model.

Lakitu and Rio now use retained photo-guided character prefabs too, with separate coat textures, articulated legs, tail motion and blinking. The shared scenery style adds warm textured floors/walls/paths, rounded furniture, windows, fences, plants, cottage roofs and park/school details. [Cat resources](Assets/Art/Cats/README.md) · [Scenery palette and editing](Assets/Art/Scenery/README.md). Existing movement, solid routes, missions and saves are preserved; added scenery props are cosmetic.

Three saved camera views now let you choose a Cozy overview, Explore orbit or Low pet-level view. Analog walking, smoother pet turns/stops and ground-aware paw contacts improve movement. See [camera/controller controls and editable motion profiles](docs/CAMERA_MOTION.md).

## Quick start

**Ready-to-play local Windows build:** close an older copy, then double-click **Play Cheweenie** on the desktop or `PLAY.cmd` in this folder. The current package is `builds/FamilyCameosWindows-0.8.1/Cheweenie.exe`, also promoted to `builds/Current` when that folder is not in use. Unity can stay closed. Extract the entire `builds/Codename-Cheweenie-Family-Cameos-0.8.1.zip` to play on another Windows PC.

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
| Cycle camera | V | R3 (press right stick) |
| Look around | Hold right mouse and drag | Right stick |
| Recenter camera | Home | RB / R1 |

Named Input System controls support Xbox and PlayStation devices. Settings provide label presets and button rebinding, saved between sessions. Menus use arrows/D-pad/stick to navigate, Enter/A/Cross to confirm and Escape/B/Circle to go back. The Camera settings row cycles views with confirm and changes distance with Left/Right. Explore/Low movement follows the camera direction; a light stick push walks slowly. Custom action bindings take priority over camera utility buttons.

Music plays on the title and in the world, fading to a brighter loop outdoors. Settings offers separate **Music** and **Sound effects** volumes; use Left/Right or horizontal stick input to adjust. Stop and restart Play after updating scripts, and ensure Game view **Mute Audio** is off. See [audio assets and controls](docs/AUDIO.md).

## Implemented foundations

The **neighbourhood extension** adds an open east garden gate, a walking street, three houses, a mailbox, flower garden, park/pond, benches, signs and a live map. Explore immediately or collect three sniff stamps and check in at the welcome-home sign. Existing progress remains compatible. See [neighbourhood route](docs/NEIGHBOURHOOD.md). The default layout is fictional. An ignored private map resource can replace it with real local streets, a park and a school stop; see [private local maps](docs/LOCAL_MAPS.md).

- **M1:** pure quest progression, strict Cozy completion, ground-probed jumping, explicit input axes, EditMode tests and a PlayMode smoke test.
- **M2:** configurable canine movement, procedural four-paw animation, optional Animator hooks, and an import-ready custom dog prefab pipeline.
- **M3:** modular schematic room definitions, upstairs loft, railed ramp, editable dimensions and optional photo panels.
- **M4:** distinct cat state machines, friendship, companion quests, obstacle checks and Lakitu following.
- **M5:** blanket resting, Cheween in Between cuddle placeholders, original synthesized chime, optional sparkles, versioned save data, pause/settings menu and keyboard/controller button rebinding.
- **M6:** a schematic backyard with stepping stones, a raised lookout, a secret hedge passage and a ribbon-recovery mission. It unlocks after the home quest; explore three landmarks in any order, find Rio's ribbon and bring it home. Companion dialogue, marker states, sight-line interaction checks and saved progress support the new loop. See [backyard route and verification](docs/BACKYARD.md).

Lakitu and Rio use the supplied photos for their distinct black-and-white coat patterns in both the cover and 3D visuals. Marty uses the saved model described above. All three remain stylized approximations; unseen anatomy is inferred. House geometry and people remain placeholders. No exact floor plan or user-specific house measurements are assumed. Final art, lighting and expanded world content remain future work.

The title and settings share original cover concept artwork, a live wordmark, a paw mark, cream text and amber accents. The title freezes gameplay until Start/Continue; title settings return to the paused title. Mouse, keyboard and controller menu navigation are supported in source. See [branding and artwork provenance](docs/BRANDING.md).

## Verification status

The 0.8.1 cameo update passes 12 native PlayMode tests and 33 Windows cameo/input/home checks with zero runtime errors. Both controller encounters and keyboard replay were verified. [Cameo evidence](docs/evidence/family-cameos.json).

On **2026-10-29**, **64 native Unity EditMode tests and 12 PlayMode tests passed** in an isolated Unity 6000.3.10f1 project. Coverage includes retained model geometry/resources, scenery, quests/saves/audio, original scene presentation, controller-only menus, camera orbit/recenter, pause input gating, view-relative analog movement and planted-paw stability. **145 standalone checks pass**; runtime/editor/test compilation has zero warnings/errors. Eleven packages and 229 local asset/metadata entries were audited.

The Windows 0.8.0 full adventure passes **51 checks**, including both Cozy moves, ribbon delivery, the local park/school patrol, return indoors and home recovery. Separate launches pass ten save/restart checks and 32 menu/input/visual checks at each of 720p and 1080p. The full run averaged 59.92 FPS here with zero runtime errors. See [actual validation results and evidence](docs/VALIDATION.md). Tests use virtual input devices; physical controller hardware and subjective speaker mix remain human checks. Pet motion is procedural animation of articulated static meshes; final likeness and a fully skinned animation pipeline remain future art work. House layout and people remain schematic.

See [camera/controller controls and motion tuning](docs/CAMERA_MOTION.md), [validation and Windows build instructions](docs/VALIDATION.md), [Marty art import](docs/ART_IMPORT.md), [editable headquarters](docs/HEADQUARTERS.md), [architecture](docs/ARCHITECTURE.md), and [development backlog](docs/CODEX_HANDOFF.md). Read AGENTS.md before changing the project.
