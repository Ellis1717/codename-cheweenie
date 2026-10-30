# Separate graphics experiment

These optional experiments accompany the stable **Codename .... Cheweenie** 0.9.0 game. They are not replacement releases. The original checkout, desktop Play Cheweenie shortcut, current build, models and saves are preserved. The initial Unity/Unreal 4.27 comparison was merged in PR #17; the UE5 preview is a subsequent, separate change.

## Try the Unity lab

Run `LAB_PLAY.cmd` in this worktree. It launches `builds/VisualLab/CheweenieLab.exe -force-d3d11 -cheweenie-visual-lab`. `LAB_BASELINE.cmd` launches the same isolated player without the visual enhancements. Close one lab mode before opening the other. The ordinary game can still be launched from its original shortcut.

The lab build uses a different company/product identity, **CheweenieGraphicsLab / Codename .... Cheweenie Visual Lab**, so its PlayerPrefs/progress are separate. It contains the original adventure with the public fictional neighbourhood. The private personal-map resource was not copied into the experiment. The original game's personal neighbourhood remains in its existing build.

Keyboard and named Xbox/PlayStation controls remain. Start an adventure and use V/R3 to inspect the pets in Explore/Low views. The lab is opt-in through its launch flag. Pressing Play normally in this branch's editor retains the default presentation; use the dedicated build/launcher for the experiment.

## What changed

- Marty uses the retained photo-guided anatomical alternative in the flagged lab, leaving the original Explorer OBJ/GLB/default prefab intact.
- Three pets gain procedural short-fur shell surfaces attached to the existing articulated joints. These are real 3D surfaces, not picture billboards. Eye surfaces are more reflective, with smaller eye details in the lab.
- Linear lighting, HDR, soft shadows, a local reflection probe, warm/cool fill lights and a lightweight filmic image effect change the presentation.
- A folded blue canopy, woven normal detail and small fairy lights add depth to the blanket fort. Added scenery has no gameplay collision.
- The lab restores changed quality settings when torn down. Actor Rigidbody/capsules, quests and bindings remain separate from its visual children.

An initial DirectX 12 capture crashed inside D3D12Core. The lab is packaged for **DirectX 11** and its launchers request that backend explicitly. The release game graphics API was not changed. The final DX11 menu/home runs completed successfully. Earlier failures are retained locally in ignored TestResults rather than represented as passing checks.

## Actual verification

- **65 native EditMode and 14 native PlayMode tests passed** on Unity 6000.3.10f1. New coverage checks fur extrusion bounds/source-mesh preservation, opt-in defaults, physics preservation and quality restoration; prior controller/menu/art/quest/save/audio tests remain.
- **43 enhanced-player graphics/menu checks**, **41 baseline menu checks** and **47 enhanced-player home/cameo/input checks** passed at 1280×720, with zero reported runtime errors on the final DX11 build. These use virtual keyboard/mouse/gamepad input; physical hardware remains a human check.
- Eleven package versions and 242 public asset/folder metadata entries were audited. All 139 packaged player files match the tested build by SHA-256.
- Stable 0.9.0 main HEAD, PLAY.cmd, default Marty prefab, package manifest, project settings and Current player assembly hashes were checked before and after. None changed.

[Enhanced report](evidence/visual-lab-menu.json) · [baseline report](evidence/visual-lab-baseline-menu.json) · [home report](evidence/visual-lab-home.json).

Actual player captures: [Marty](screenshots/visual-lab-marty.png), [Lakitu](screenshots/visual-lab-lakitu.png), [fort](screenshots/visual-lab-fort.png), [baseline fort](screenshots/visual-lab-baseline-fort.png). The fort capture temporarily poses the three real 3D pets for comparison and restores them afterward; it is not a new scripted mission.

## The remaining art gap

This is a modest rendering/anatomy experiment. It still looks stylized and does not reproduce the landing-page illustration. The current pets are articulated static meshes rather than detailed, skinned, groomed characters. Fur shells add surface softness but cannot repair anatomy, facial structure or joint deformation. House/people geometry remains schematic.

The next useful asset milestone is one carefully authored Marty rig: connected sculpted body/face, smaller anatomical eyes with separate iris/cornea, a skin-weighted skeleton, walk/trot/run/idle/turn/Cozy clips, PBR coat maps and hair cards/groom with LODs. Approve that playable character before producing matching Lakitu/Rio rigs. Preserve Lakitu's asymmetric black/white face and Rio's black face/white chin/bib. Then improve furnishings, draped fabric, vegetation, material detail and indirect lighting in a small finished room before applying the style across the world.

## Unreal comparison

`Unreal/VisualLab/CheweenieVisualLab.uproject` is a separate **Unreal 4.27.2** asset/lighting spike made with the installed engine. It retains three imported static pet meshes, their coat textures/materials, source OBJ/MTL/JSON exports and a generated comparison room. It has a default free-camera pawn; it does **not** port the Unity quests, saves, Marty controller, pets' animation or family game UI. See [Unreal instructions](../Unreal/VisualLab/README.md).

UE5 was not installed during the original 4.27 audit. Unreal Engine 5.8.4 has since been installed separately on D:. The 4.27 spike does not exercise Lumen/Nanite. The new UE5 project below retains a distinct copy; the stable Unity release remains available.

The installed Unreal editor successfully rebuilt/saved a 12-actor room with exactly three static pet actors, and a standalone editor-game launch produced an [actual material-ready render](screenshots/unreal-comparison.png). Early captures showed compiling placeholder materials and overexposure; the fill light was reduced. Shader compilation and lighting remain a rough benchmark setup, not a finished art scene. Unreal gameplay/controller interaction was not acceptance-tested.

Unreal can help with indirect lighting/reflections, but the same simplified mesh remains simplified in another engine. The main next investment is character/environment asset quality. Engine choice should follow that controlled prototype rather than a full rewrite first.

Primary references: [Epic: Lumen](https://dev.epicgames.com/documentation/en-us/unreal-engine/lumen-technical-details-in-unreal-engine), [Epic: hardware requirements](https://dev.epicgames.com/documentation/en-us/unreal-engine/hardware-and-software-specifications-for-unreal-engine), [Epic: editor Python](https://dev.epicgames.com/documentation/en-us/unreal-engine/scripting-the-editor-using-python?application_version=4.27), [Unity: Built-In surface shaders](https://docs.unity.com/en-us/engine/6000.3/manual/materials-and-shaders/shaders/writing-shaders-birp/writing-surface/sl-surface).

## UE5 room preview — 0.2.0

Run `LAB_UE5.cmd` for a separate **Unreal 5.8.4** editor-game viewer, or open `Unreal/UE5VisualLab/CheweenieUE5Lab.uproject`. This requires the installed engine; it is not a packaged standalone game. The Unity release and original 4.27 project remain unchanged.

The UE5 copy retains Marty, Lakitu and Rio's static meshes and coat textures, and rebuilds its own material graphs from the retained source colour/texture metadata. A 59-actor room adds oak floorboards, textured plaster, a framed window, woven blue rug, rounded sofa/cushions, a side table and planter. Daylight, a soft window source, a warm lamp and a subtle frontal fill illuminate the pets. Project-local settings enable DX12/SM6, software Lumen GI/reflections, mesh distance fields, virtual shadows and TSR. Hardware ray tracing is disabled; Nanite and grooms are not introduced.

Native generation on 5.8.4 saved the map, found exactly three pet actors, verified all material slots point to the UE5 material folder, and confirmed GI/reflections/distance-field console settings are enabled. [Generated asset/settings report](evidence/ue5-preview.json). A native DX12 editor-game process reached play startup. Keyboard interaction and physical controllers could not be exercised because Windows desktop capture/input became unavailable. Bindings were inspected in the installed `DefaultPawn.cpp`; left/right sticks, WASD, mouse and vertical viewer controls are documented in the [UE5 README](../Unreal/UE5VisualLab/README.md).

Unreal's own offscreen DX12 render capture was used for visual inspection. Exposure and game-view overlays were corrected through actual captures; the final image is retained below. The capture waits for viewport warmup and 64 screenshot accumulation frames, then exits. This checks rendering, not physical controller behavior or gameplay performance. Early shader/PSO compilation can cause startup hitches. No frame-rate target is claimed.

[Actual UE5 room render](screenshots/ue5-preview.png). The pets are still stylized static exports, without a skin-weighted skeleton, groom or authored walk/Cozy animation. The free camera is the viewer, not Marty. This is a rendering/scenery comparison, not the complete Unity adventure port. The next meaningful Unreal milestone remains one properly rigged, moving Marty character before committing to a full rewrite.

Snapshot export retains the UE5 launcher and binary Content assets while excluding Saved, Intermediate, Binaries and caches. The export check found the UE5 map as base64 and no generated engine folders. Unity source, packages, ProjectSettings and the 4.27 source/assets have no diff; Unity's existing test suite was not rerun for this isolated Unreal change.

## UE5 movement source — not yet playable

`Unreal/UE5MovementLab` prepares a separate native Character/GameMode, Enhanced Input bindings, ground movement, camera-relative walking/hurrying, three camera presets, recentering, return-to-spawn, room-edge boundaries and a controls HUD. It reuses the retained Marty static mesh and a separate generated copy of the preview Content. The content-only preview has no new native module dependency. Unity, UE4 and UE5 preview source/assets remain unchanged.

**The native build failed before C++ compilation.** UnrealBuildTool 5.8.4 explicitly bans the installed MSVC v14.42 (banned range 14.40–14.43). Windows SDK 10.0.22621.0 is present. An attempted side-by-side v14.38 installation returned 5007: the Visual Studio installer requires elevation. No compiler-ban override was applied. Install a supported toolchain through the elevated Visual Studio Installer, then follow the [movement build and acceptance instructions](../Unreal/UE5MovementLab/README.md).

Both new PowerShell scripts passed syntax parsing; the native module/plugin manifest was checked, including AndroidFileServer remaining disabled. Git comparison confirms no changes to Unity Assets/Packages/ProjectSettings, UE4, or the existing UE5 preview. Unity tests were not rerun for this isolated source addition. Native code compilation, traversal, keyboard/gamepad interaction and animation remain **unverified**. The mesh has no walk animation; a proper authored rig is still outstanding. The new launcher refuses to open without a native DLL. This work is a draft, not a replacement family release.

A read-only native 5.8.4 asset audit exited successfully, loaded the 59-actor comparison room and verified all three pets' material slots (8 Marty, 10 Lakitu, 8 Rio). [Asset report](evidence/ue5-movement-assets.json). Measured display bounds inform a provisional 32% pet scale in the movement source; the comparison room's scale is unchanged. Snapshot export checks retain the new launcher/native sources and exclude generated movement Content, Saved, Intermediate and Binaries. These passes do not establish native gameplay correctness.

## Unreal is now the active target

The owner has set Unity aside for continued Unreal development. AGENTS.md and [UNREAL_HANDOFF.md](UNREAL_HANDOFF.md) now prioritize Unreal traversal, Marty animation, companions, the family demo loop, scenery and packaging, in that order. Unity remains a preserved fallback; its assets/settings/source and both existing Unreal comparison projects have no changes.

The elevated side-by-side v14.38 compiler installation succeeded (installer exit 0). A real native build selected MSVC 14.38.33144 with SDK 10.0.22621.0 but failed while compiling Unreal's shared precompiled headers: C7539 in ContainerAllocationPolicies.h and C4346 in UnifiedError.h, followed by cascading errors. Thus passing the engine's minimum-version detection did not establish compatibility. Requesting v14.44 through the installed Visual Studio 17.12 product graph found no matching package. The separate current Build Tools download/install attempt was rejected by automatic approval review as "blocked by policy". Current v14.44+ tools must be installed before resuming native compilation; no engine-header patch or compiler-ban override was applied.

An opt-in runtime harness now prepares twelve checks for actual grounded movement, synthetic keyboard/gamepad bindings, stopping, camera switching, home recovery and sustained boundary collision. TEST.ps1 requires a fresh native JSON report; its rendered mode additionally requires a fresh gameplay capture. **The harness has not run successfully because no native module has compiled.** The three movement PowerShell scripts parse, missing-compiler/missing-module guards pass, and snapshot checks retain the new handoff/test source while excluding generated files. Build uses local execution rather than the installed external build accelerator. Native compile, runtime and physical controller checks remain open.
