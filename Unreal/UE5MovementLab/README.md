# Codename .... Cheweenie — Marty movement prototype

**Prepared source; native compilation and gameplay are currently unverified.** This separate project depends on the UE5 room preview in PR #18. Use `PLAY.cmd` for the family adventure and `LAB_UE5.cmd` for the static room viewer.

## Build prerequisite

Unreal Engine 5.8.4 is installed at `D:\Epic Games\UE_5.8`. Microsoft compiler v14.42 is explicitly **banned by UnrealBuildTool** because of compiler defects. A side-by-side v14.38 installation subsequently succeeded, but the real native build failed inside the engine's shared precompiled headers (C7539/C4346 and follow-on errors). It is insufficient for this project despite passing the engine's minimum-version check. Windows SDK 10.0.22621.0 is present.

Install current **Visual Studio 2022 C++ Build Tools with MSVC v14.44**, or a supported newer compiler. Updating Visual Studio 2022 to 17.14 makes its v14.44 tools available; the existing 17.12 product graph did not contain the requested v14.44 component. A separate current Build Tools installation can instead be placed on D:. The attempt to download/install that separate package was rejected by automatic approval review ("blocked by policy"). No compiler-ban override or engine-source workaround is used. The build script checks for v14.44+ before invoking UnrealBuildTool, which still performs its own detailed compatibility checks.

Microsoft references: [install C++ Build Tools](https://learn.microsoft.com/en-us/cpp/overview/acquire-msvc?view=msvc-170), [installer command-line options](https://learn.microsoft.com/en-us/visualstudio/install/use-command-line-parameters-to-install-visual-studio?view=visualstudio).

From PowerShell in this folder:

```powershell
powershell -NoProfile -File .\BUILD.ps1
powershell -NoProfile -File .\OPEN.ps1
```

Both accept `-EngineRoot`. The build first copies the retained preview Content into a separate ignored Content folder, only if absent. Native DLLs/build caches are ignored. The copied Content can be edited without changing the preview; to intentionally refresh it, archive your edits and remove that copy before rebuilding. Original assets remain versioned in `../UE5VisualLab/Content` and original mesh exports in `../VisualLab/SourceAssets`.

After a successful build, root `LAB_UE5_MARTY.cmd` opens the movement mode. It refuses to open without its native module. This is an editor-game prototype requiring installed Unreal, not a packaged standalone release.

## Implemented in source

Marty is a Character with a ground capsule, acceleration/braking, camera-relative analog movement and smooth turning. The retained UE5 Marty mesh follows that capsule. The duplicate static Marty is removed only in this mode; Lakitu and Rio remain static. Four runtime boundaries protect the open room edges. A spring-arm camera avoids scenery, offers three distances/heights, and recenters on demand. An on-screen control panel stays visible.

Native asset inspection measured the display Marty at 212 × 92 × 141 cm. All pets use 32% of that display scale only in this movement mode (Marty approximately 68 cm long, 45 cm high). These are provisional proportions, not measurements of the real animals. Spawn position, capsule dimensions, speeds and camera distances are centralized in `MartyCharacter.cpp` for tuning after the first play test.

| Action | Keyboard / mouse | Xbox / PlayStation |
|---|---|---|
| Move | WASD or arrows | Left stick |
| Look | Mouse | Right stick |
| Hurry while held | Left Shift | A / Cross |
| Cycle camera | V | Right stick click / R3 |
| Recenter | R | RB / R1 |
| Return to spawn | H | Menu / Options |
| Exit | Alt+F4 | Close window |

Bindings use Enhanced Input. Diagonal motion is clamped; analog magnitude is retained. Controller wording identifies the same Unreal gamepad keys; physical PlayStation support also depends on how Windows exposes the device.

**The mesh is still static.** This milestone prepares traversal, not realistic leg motion. No skin weights, walk clips, groom, quests, saves or full Unity UI have been ported. The following character-art milestone remains a properly authored Marty rig, before equivalent cat work.

## Required acceptance after compiler installation

1. Run BUILD.ps1 and fix any native compiler errors before calling this playable.
2. Launch, confirm exactly one Marty and both cats, inspect materials and floor contact.
3. Walk/turn slowly, hurry, release inputs, and verify acceleration/braking and room/furniture collision. Confirm diagonal speed and analog walking.
4. Orbit, cycle all three views, recenter and return home. Check the camera near walls and sofa.
5. Repeat with a physical Xbox/PlayStation controller, then inspect the native log for errors. Capture an actual gameplay image.

The read-only `Scripts/audit_assets.py` runs in the content-only preview project; its output audits assets, not native code or input. Actual verification is recorded in `docs/VISUAL_REALISM_LAB.md`.

`TEST.ps1` prepares an opt-in `-CheweenieSmoke` native runtime run with twelve synthetic input/ground/boundary checks and a fresh JSON report. `TEST.ps1 -Render` additionally requires a fresh gameplay screenshot from the native DX12 offscreen game. This harness has **not executed successfully** because the native module has not compiled. Neither mode substitutes for testing a physical controller. Build uses Unreal's local executor (`-NoXGE`), avoiding the installed external build accelerator.
