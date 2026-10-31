# Codename .... Cheweenie 🐾

[Photo-derived character likeness brief](docs/PET_REFERENCE_BRIEF.md): observed features, marking maps and unknowns for Marty, Lakitu and Rio, prepared for the next realism/rigging pass. The current game models are unchanged by this reference extraction.

**Cozy crew 0.9:** Lakitu and Rio can follow, stay, come when called, share head bumps and build saved friendships. K / D-pad Down opens crew commands. Routing reuses a cached street graph and skips unchanged/paused work; the wide camera now frames the group. [Companions, controls and limitations](docs/UNREAL_COMPANIONS.md). Cat limb/fur rigging remains future work; their original meshes and markings are retained.

The final 0.9 package and regressions passed 288 native assertions, three audio waveform checks and a full-HD coat check. [Actual results](docs/evidence/ue5-09-build.json), [crew in game](docs/screenshots/ue5-09-companion-crew.png). Physical controllers and whole-game frame-time performance remain unverified.

**Navigation 0.8:** north-up maps, separate Marty/camera heading markers, a compass, saved destinations, location finding and street guidance. M / D-pad Up opens the map. Pause → Controls & cameras offers 24 independent navigation, pet, camera and sound switches. [Controls and map guide](docs/UNREAL_NAVIGATION.md).

The final 0.8 package and regressions passed 219 native assertions, three audio waveform checks and a full-HD coat-render check. [Actual results](docs/evidence/ue5-08-build.json). Physical controllers remain unverified; earlier releases and Unity are retained.

**Marty 0.7:** more detailed short-coat geometry and 4K texture maps, visible health/energy bars, backpack storage, lying down/getting up, roll-over and shake-paw tricks, three paces and an optional two-stick body-sway mode. C / B-Circle opens tricks and controls; Tab / View-Touchpad opens the bag. [Controls, art sources and limitations](docs/UNREAL_MARTY_07.md).

The 0.7 native builds/package passed 151 synthetic gameplay assertions plus audio and full-HD coat-render checks. [Actual results and verification limits](docs/evidence/ue5-07-build.json). Physical controllers remain unverified; previous releases and Unity are preserved.

**Merged main verified (2026-10-30):** Unreal PRs #18–#23 are merged. A fresh Windows package built from main passed 115 synthetic keyboard/gamepad gameplay, persistence, recovery and audio assertions, plus three bark/music/mute waveform checks. Actual title/kitchen captures were reviewed. [Post-merge results](docs/evidence/ue5-main-merge.json). Physical controllers and subjective listening remain unverified.

**Active development now targets Unreal Engine 5.8.4.** The Unity release is retained as a playable fallback. Follow the [Unreal development handoff](docs/UNREAL_HANDOFF.md) for the ordered gameplay, character, scenery and packaging milestones.

**Audio, 0.6.1:** B / LB-L1 barks; paw steps, digging, chewing, toy pickup/drop, jumping and landing have dedicated sounds. Retained home/backyard music loops follow the location, with separate saved music/effects volumes. [Audio sources and verification](docs/UNREAL_AUDIO.md).

**Pet sandbox, 0.6:** Marty has energy and tired/normal/excited moods. Pick up, carry, drop and bury toys; dig them up again; jump or hop onto the sofa; watch original Paw TV to rest; and eat at the kitchen bowl. [Controls, needs and save rules](docs/UNREAL_DOG_LIFE.md). New inputs: Space/Y-Triangle jumps, F/RT-R2 digs, Q/LT-L2 drops, E/X-Square interacts. The validated launcher retains 0.5 and 0.4 as fallbacks.

**Marty graphics and movement, 0.5:** a new cover-guided skin-weighted character has smooth leg joints, grounded paw steps, flexible ear bases/tips, coat grain and wet eye/nose materials. Press V/R3 three times in the patrol for the close camera, then orbit to inspect him. [Art sources, tuning and limits](docs/UNREAL_MARTY_ART.md). Cats retain their previous meshes; further likeness and realism work remains.

**Outside headquarters:** use the blue door with E/X-Square or choose Explore the neighbourhood. Roads, nine house exteriors, eight editable neighbour dogs, a park and school extend the demo. The launcher uses the retained private street layout when present; the public package has a schematic fallback. [Outdoor controls and neighbour names](docs/UNREAL_NEIGHBOURHOOD.md).

**Optional graphics labs — the stable 0.9.0 game is preserved in the original checkout.** Use `LAB_PLAY.cmd` for the separate Unity graphics build and `LAB_BASELINE.cmd` for its baseline mode, with separate saves. `LAB_UE5.cmd` opens a separate Unreal 5.8.4 room preview with the three static pets, Lumen lighting and cozy furnishings (installed UE5 editor required). The original Unreal 4.27 project remains under `Unreal/VisualLab`; the Unity game has not been converted. [UE5 preview instructions](Unreal/UE5VisualLab/README.md) · [Actual changes, tests, captures and remaining realism work](docs/VISUAL_REALISM_LAB.md).

The preserved Unity adventure stars Marty the Chiweenie, Lakitu (Keetz / Keeterman), and Rio (Sis / Little Miss / Miss Priss).

**Unreal Cozy Patrol 0.4 is now a standalone Windows demo.** Use UNREAL_PLAY.cmd or **Play Cheweenie Unreal** on the desktop. Find three snacks, visit Lakitu and Rio, then get Cheeween Cozy at the blanket fort. Includes title/help/pause/sound menus, original audio, checkpoints, replay and the Fox/Leo cameos. The packaged executable passed 25 patrol plus six persistence/replay checks using synthetic input. [Play and extension guide](docs/UNREAL_GAMEPLAY.md) · [Actual title](docs/screenshots/ue5-adventure-packaged-title.png) · [Actual gameplay](docs/screenshots/ue5-adventure-packaged-play.png). Native source and owned assets are retained; models are still static, and physical hardware/audio/likeness checks remain open.

**Illustrated field guide (0.9.0):** the expanded title menu includes Controls & camera, Adventure guide, Meet the crew and About. Ten guide pages cover current bindings, cameras, recovery, missions and character biographies, with three new reference-guided illustrations. Open help with **F1** during play or **F1 / Y / Triangle** from paused Settings. [Menu guide and artwork](docs/MENU_GUIDE.md).

**Family cameos (0.8.1):** sniff the backyard zone for **Fox Ellis, Chief Sniff Inspector: “Yep. Definitely grass.”** Get Cheeween Cozy at the blanket fort for **Leo Ellis, Blanket Boss: “Five more minutes!” Marty agrees.** Both messages stay visible for eight seconds and replay when you repeat the encounter after completing it. Cat friendship rewards still apply.

Marty now loads the original **Marty Explorer** mesh from your supplied character archive. His geometry and ten source colours are retained, with added leg, head, ear, tail and blink motion. Original OBJ/GLB, the Unity prefab, meshes, materials, importer and animation source are version controlled; a photo-guided alternative is retained too. See [Marty resources and editing](Assets/Art/Marty/README.md). The concept sheet has more detail than the supplied static mesh; it is an art target rather than a render of this model.

Lakitu and Rio now use retained photo-guided character prefabs too, with separate coat textures, articulated legs, tail motion and blinking. The shared scenery style adds warm textured floors/walls/paths, rounded furniture, windows, fences, plants, cottage roofs and park/school details. [Cat resources](Assets/Art/Cats/README.md) · [Scenery palette and editing](Assets/Art/Scenery/README.md). Existing movement, solid routes, missions and saves are preserved; added scenery props are cosmetic.

Three saved camera views now let you choose a Cozy overview, Explore orbit or Low pet-level view. Analog walking, smoother pet turns/stops and ground-aware paw contacts improve movement. See [camera/controller controls and editable motion profiles](docs/CAMERA_MOTION.md).

## Quick start

**Ready-to-play local Windows build:** close an older copy, then double-click **Play Cheweenie** on the desktop or `PLAY.cmd` in this folder. The current package is `builds/FieldGuideWindows-0.9.0/Cheweenie.exe`, also promoted to `builds/Current` when that folder is not in use. Unity can stay closed. Extract the entire `builds/Codename-Cheweenie-Illustrated-Field-Guide-0.9.0.zip` to play on another Windows PC.

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
| Field guide | F1 during play or Settings | Y / Triangle from paused Settings |

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

The 0.9.0 menu/art update passes **13 native PlayMode tests**, **44 Windows menu/input/visual checks at each of 720p and 1080p**, and **45 playable home/cameo checks**, with zero runtime errors. Actual renders and the three new images are retained with [evidence](docs/VALIDATION.md) and [full generation prompts](docs/MENU_ART_PROMPTS.md).


The 0.8.1 cameo update passes 12 native PlayMode tests and 33 Windows cameo/input/home checks with zero runtime errors. Both controller encounters and keyboard replay were verified. [Cameo evidence](docs/evidence/family-cameos.json).

On **2026-10-29**, **64 native Unity EditMode tests and 12 PlayMode tests passed** in an isolated Unity 6000.3.10f1 project. Coverage includes retained model geometry/resources, scenery, quests/saves/audio, original scene presentation, controller-only menus, camera orbit/recenter, pause input gating, view-relative analog movement and planted-paw stability. **145 standalone checks pass**; runtime/editor/test compilation has zero warnings/errors. Eleven packages and 229 local asset/metadata entries were audited.

The Windows 0.8.0 full adventure passes **51 checks**, including both Cozy moves, ribbon delivery, the local park/school patrol, return indoors and home recovery. Separate launches pass ten save/restart checks and 32 menu/input/visual checks at each of 720p and 1080p. The full run averaged 59.92 FPS here with zero runtime errors. See [actual validation results and evidence](docs/VALIDATION.md). Tests use virtual input devices; physical controller hardware and subjective speaker mix remain human checks. Pet motion is procedural animation of articulated static meshes; final likeness and a fully skinned animation pipeline remain future art work. House layout and people remain schematic.

See [camera/controller controls and motion tuning](docs/CAMERA_MOTION.md), [validation and Windows build instructions](docs/VALIDATION.md), [Marty art import](docs/ART_IMPORT.md), [editable headquarters](docs/HEADQUARTERS.md), [architecture](docs/ARCHITECTURE.md), and [development backlog](docs/CODEX_HANDOFF.md). Read AGENTS.md before changing the project.
