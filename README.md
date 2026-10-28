# Codename .... Cheweenie 🐾

A cozy, playful 3D Unity adventure starring Marty the Chiweenie, Lakitu (Keetz / Keeterman) and Rio (Sis / Little Miss / Miss Priss).

## Quick start
1. Open this repository as a project in **Unity 6.3 LTS** (Universal Render Pipeline optional; built-in renderer works with this prototype).
2. Open the project and press **Play** in any empty scene. The prototype world is built at runtime; no downloaded assets are needed.
3. Walk with **WASD / left stick**, **Space / gamepad South (A/X)** to jump, **E / gamepad West (X/Square)** to interact, **Left Shift / gamepad East (B/Circle)** to sprint, and **C / gamepad North (Y/Triangle)** for Cheeween Cozy. The camera follows automatically.
4. Find each cat, visit the backyard, then return to the blanket fort.

## Current prototype
- Procedural two-story family-inspired headquarters, backyard and interactable blanket fort (placeholder until a house floor plan/photos are translated into exact geometry).
- Controllable third-person Marty placeholder mesh, camera follow, controller and keyboard input, jump and sprint.
- Cats Lakitu and Rio, proximity interaction, quest objectives and simple HUD.
- Cozy move, mission tracking and a clear progression path for art/animation upgrades.

## Development handoff
Read **AGENTS.md** before coding and **docs/CODEX_HANDOFF.md** for the ordered backlog. See also **docs/ARCHITECTURE.md**.

This is a **source-first prototype**, not a compiled game. Unity editor verification, gamepad mapping on target hardware and imported dog/cat assets remain to be tested.

## M1 validation
## M2 character foundation
## M3 headquarters
`HouseLayout` defines editable room sizes, story height, photo panels and ramp endpoints. The runtime builder creates ground rooms, an upstairs loft and railed ramp; all are explicitly schematic. See `docs/HEADQUARTERS.md` for configuration. Camera collision avoids clipping through walls. Source compilation passed; ramp traversal and camera behavior await Unity license activation.

Marty has an independent configurable physics controller and a procedural dog rig with animated paws, muzzle, floppy ears and tail. `MartyTuning` and `MartyVisual` Resources assets support tuning and custom FBX prefabs; see `docs/ART_IMPORT.md`. Explicit joystick axes avoid dependence on Unity's generated defaults. Keyboard uses WASD/arrows. Animation supports optional Speed/Cozy parameters with root motion disabled.

Validation: C# assembly compilation and 13 pure quest checks passed after M1. Unity license failure still prevents import/play/controller validation of M2.

Quest order and the fort's Cozy requirement now live in pure C# logic with EditMode tests. Ground probing replaces the jump velocity heuristic. Legacy input and Unity Test Framework are declared dependencies. Shader selection follows the active render pipeline.

On 2026-10-28, Unity 6000.3.10f1 batch startup failed with exit 198: no valid editor license. Compilation, PlayMode, keyboard/gamepad, physics and screenshots remain pending. `Validation/Compile.csproj` checks C# against installed Unity assemblies; `Validation/QuestChecks.csproj` exercises quest rules outside Unity. These do not replace Unity gameplay tests.
