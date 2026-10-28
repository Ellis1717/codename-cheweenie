# Codename .... Cheweenie 🐾

A cozy Unity adventure starring Marty the Chiweenie, Lakitu (Keetz / Keeterman), and Rio (Sis / Little Miss / Miss Priss).

## Quick start
1. Open this folder in **Unity 6.3 LTS** with an active editor license and resolve the manifest packages.
2. Use **Input Manager (Old)** or **Both** for Active Input Handling. Run **Cheweenie > Validate project configuration**.
3. Press Play in an empty scene. The source-first prototype builds its world at runtime.
4. Find Lakitu, find Rio, sniff the backyard, then use **Cheeween Cozy** at the blanket fort.

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

## Implemented foundations
- **M1:** pure quest progression, strict Cozy completion, ground-probed jumping, explicit input axes, EditMode tests and a PlayMode smoke test.
- **M2:** configurable canine movement, procedural four-paw animation, optional Animator hooks, and an import-ready custom dog prefab pipeline.
- **M3:** modular schematic room definitions, upstairs loft, railed ramp, editable dimensions and optional photo panels.
- **M4:** distinct cat state machines, friendship, companion quests, obstacle checks and Lakitu following.
- **M5:** blanket resting, Cheween in Between cuddle placeholders, original synthesized chime, optional sparkles, versioned save data, pause/settings menu and keyboard/controller button rebinding.

House geometry, character likenesses and people are placeholders. No exact floor plan or user-specific house measurements are assumed. User art, rigging, final lighting and expanded world content remain future work.

## Verification status
On **2026-10-28**, runtime, editor utility and test sources compiled against installed Unity 6.3 assemblies targeting .NET Standard 2.1 with **zero warnings/errors**. **13 pure quest checks passed outside Unity**.

Unity batch startup exited **198: no valid editor license**, before project import or compilation. Unity Test Runner, PlayMode, keyboard/gamepad hardware, visuals/audio, package resolution and Windows build are **unverified**. Source milestones do not yet meet the gameplay definition of done.

See [validation and Windows build instructions](docs/VALIDATION.md), [Marty art import](docs/ART_IMPORT.md), [editable headquarters](docs/HEADQUARTERS.md), [architecture](docs/ARCHITECTURE.md), and [development backlog](docs/CODEX_HANDOFF.md). Read AGENTS.md before changing the project.
