# Codename .... Cheweenie — Marty movement prototype

**Native compilation and sixteen gameplay checks pass in both headless and rendered DX12 runs.** This separate project depends on the UE5 room preview in PR #18. Run `LAB_UE5_MARTY.cmd` for the controllable Unreal prototype after building. On this machine, **Play Cheweenie Unreal** on the desktop starts it directly. The Unity fallback remains available through `PLAY.cmd`; `LAB_UE5.cmd` opens the static room viewer.

## Build prerequisite

Unreal Engine 5.8.4 is installed at `D:\Epic Games\UE_5.8`. The existing Microsoft Visual Studio installation has now updated to 17.14, supplying **MSVC 14.44.35229**. The successful build selected Windows SDK 10.0.22621.0. These dependencies are installed on this machine; no further compiler installation is needed here.

For another machine, install current **Visual Studio 2022 C++ Build Tools with MSVC v14.44**, or a supported newer compiler. Earlier v14.42 was banned by UnrealBuildTool, and v14.38 failed inside UE5.8's headers despite passing version detection. The previous separate installer download was blocked; updating through the existing Microsoft installer resolved the prerequisite. No compiler-ban override or engine-source workaround is used. BUILD.ps1 checks for v14.44+ before invoking UnrealBuildTool, which performs its own detailed compatibility checks.

Microsoft references: [install C++ Build Tools](https://learn.microsoft.com/en-us/cpp/overview/acquire-msvc?view=msvc-170), [installer command-line options](https://learn.microsoft.com/en-us/visualstudio/install/use-command-line-parameters-to-install-visual-studio?view=visualstudio).

From PowerShell in this folder:

```powershell
powershell -NoProfile -File .\BUILD.ps1
powershell -NoProfile -File .\OPEN.ps1
```

Both accept `-EngineRoot`. The build first copies the retained preview Content into a separate ignored Content folder, only if absent. Native DLLs/build caches are ignored. The copied Content can be edited without changing the preview; to intentionally refresh it, archive your edits and remove that copy before rebuilding. Original assets remain versioned in `../UE5VisualLab/Content` and original mesh exports in `../VisualLab/SourceAssets`.

After a successful build, root `LAB_UE5_MARTY.cmd` opens the movement mode. It refuses to open without its native module. This is an editor-game prototype requiring installed Unreal, not a packaged standalone release.

## Implemented in source

Marty is a Character with a ground capsule, acceleration/braking, camera-relative analog movement and smooth turning. The retained UE5 Marty mesh follows that capsule. The duplicate static Marty is removed only in this mode; Lakitu and Rio remain static. Four runtime boundaries protect the open room edges. A spring-arm camera avoids scenery, offers three distances/heights, and recenters on demand. An on-screen control panel stays visible. Runtime boxes match the upholstered sofa parts: their stretched sphere meshes had smaller simple physics shapes, and an actual character-movement test exposed the mismatch. Comparison assets remain unchanged.

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

1. Run BUILD.ps1 to compile the native module on a fresh checkout.
2. Launch, confirm exactly one Marty and both cats, inspect materials and floor contact.
3. Walk/turn slowly, hurry, release inputs, and verify acceleration/braking and room/furniture collision. Confirm diagonal speed and analog walking.
4. Orbit, cycle all three views, recenter and return home. Check the camera near walls and sofa.
5. Repeat with a physical Xbox/PlayStation controller, then inspect the native log for errors. Capture an actual gameplay image.

The read-only `Scripts/audit_assets.py` runs in the content-only preview project; its output audits assets, not native code or input. Actual verification is recorded in `docs/VISUAL_REALISM_LAB.md`.

`TEST.ps1` runs opt-in `-CheweenieSmoke` native gameplay acceptance and requires a fresh JSON report. All sixteen headless checks passed: ground contact, keyboard movement/stopping, analog stick walking/stopping, hurry press/release, camera distances, orbit/upward look/recenter, keyboard/gamepad home recovery, room bounds and actual sofa blocking. [Headless report](../../docs/evidence/ue5-movement-headless.json). `TEST.ps1 -Render` warms the game for sixty seconds and additionally requires a fresh gameplay screenshot from the native DX12 offscreen game. Neither mode substitutes for a physical controller check. Explicit Enhanced Input scales avoid inherited legacy camera multipliers.

The rendered run also passed all sixteen checks and produced this [actual gameplay capture](../../docs/screenshots/ue5-marty-movement.png). [Rendered report](../../docs/evidence/ue5-movement-rendered.json) · [native build record](../../docs/evidence/ue5-movement-build.json). Both final gameplay logs had no Error/Fatal categories. This remains a small room prototype with static models, not the full Unity adventure or a finished realistic character demonstration. No frame-rate target is claimed.

A normal interactive game process was also opened successfully. Windows desktop automation then returned `GetCursorPos failed: Access is denied (0x80070005)`, so an OS-level keyboard check could not be completed. The reports above use native synthetic input events. Select the game window and repeat the controls with your keyboard/controller for the remaining human acceptance.

Build limits concurrency to one and disables XGE/UBA acceleration (`-NoXGE -NoUBA`). An early accelerated build exhausted this machine's available memory commitment; the serial rebuild passed. No Windows page-file settings or unrelated running apps were changed.

Run `AUDIT.ps1` without compiling to audit the retained content-only preview. Native UE5.8.4 passed the three-pet/material checks and verified simple shapes plus Pawn/Camera blocking on the solid floor, blue rug, sofa seat and back wall. The wrapper requires a fresh report: UE5's Python executor can exit 0 even when a script fails. Early enum-name/type compatibility errors were detected by that freshness guard and fixed before the final passing run. This audit reads assets only; it does not establish native gameplay correctness.
