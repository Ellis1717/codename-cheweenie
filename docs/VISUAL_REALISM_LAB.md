# Separate graphics experiment

## Current demo — Cozy Patrol 0.5

Marty now uses an original cover-guided, skin-weighted 24-joint model with procedural elbow/stifle/hock/paw IK, grounded stepping, flexible ear tips and short-coat/eye/nose materials. Closed surfaces and outward normals avoid the initial open-limb/ear artifacts. Editable Blender/FBX/textures and the old static fallback are retained. V/R3 cycles a fourth close camera. This remains stylized; it does not reproduce the cover's full photographic detail. Cats remain static.

The headquarters door and title Explore option lead to a larger exterior with a reference-guided brick/blue-garage home, eight editable dog/house slots, roads, park, school and tour shortcuts. This machine's launcher loads the retained private OSM street file from ignored LocalGameData; public builds have a schematic fallback. Original private photos/address/map coordinates are not committed. Source and controls: UNREAL_MARTY_ART.md and UNREAL_NEIGHBOURHOOD.md.

Final 0.5 native Editor/Game builds and packaging passed. The standalone DX12 client passed 57 checks (14 gait, 12 exterior, 25 patrol, six persistence/replay). Four more packaged headless checks validated the private map; the earlier sixteen movement checks passed at the 60 FPS cap. Actual packaged screenshots were inspected. Early testing exposed missing skeletal material usage/save, open surfaces, signs spawned at the origin, an uncapped analog test, transient memory exhaustion, dark outdoor shadows and camera/capture timing problems. These were corrected before the final package. No system memory settings or unrelated apps were changed. A validated marker gates the desktop launcher; rebuilding falls back to preserved 0.4 until fresh reports/captures pass finalization. Physical devices, audible mix, subjective realism and measured performance remain open.

Evidence: docs/evidence/ue5-05-*.json and docs/screenshots/ue5-05-*.png. Hashes cover immutable payload only; runtime Saved files are excluded. Unity and the independent comparison projects are unchanged.

## Prior game foundation — Cozy Patrol 0.4

At the owner's request, the latest work prioritizes a complete extensible game loop. The Unreal adventure includes the branded title, mouse/controller menu handlers, help, pause/recovery, sound preferences, original music/effects, three snack pickups, ordered Lakitu/Rio interactions, Cheeween Cozy completion, Fox/Leo cameos, free roam/replay and checkpoint saves. Extra walls close the photography room and the fort rests on the floor. Models remain static; the ending is not an authored snuggle animation.

CozyAdventure owns mission/session/save/audio rules, CozyHUD owns presentation/hitboxes, and MartyCharacter retains movement/camera ownership. CozyMissionDefinition Data Assets can override stops, labels, dialogue, radii and sound names. Session commands are Blueprint-callable. [Play and extension guide](UNREAL_GAMEPLAY.md). Owned Content/Adventure assets are versioned separately from generated comparison content. Unity source/assets/settings and both independent comparison projects remain unchanged.

Native editor compilation, the full rendered patrol and the original sixteen movement checks passed. The patrol has 25 checks; cross-process persistence/replay has six, including confirmed restart retaining preferences. A first restart test needed a released-input frame between confirm keys; final runs pass. Menu review fixed volume-row selection returning to the parent menu; acceptance now exercises the HUD hitbox handler. That checks the handler, not OS mouse coordinates.

The standalone Development Win64 game built/cooked/staged/archived successfully and independently passed 25 patrol plus six persistence/replay checks under DX12. Tests drive the real pawn around the full patrol with synthetic input, rather than teleporting to collect objectives. Fresh title/play/completion captures were inspected. Packaged patrol/resume logs have no Error/Fatal categories. Muted tests verify audio assets/preferences; physical devices, audible mix and owner likeness remain human checks.

[Headless patrol](evidence/ue5-adventure-headless.json) · [rendered patrol](evidence/ue5-adventure-rendered.json) · [persistence/replay](evidence/ue5-adventure-resume.json) · [packaged patrol](evidence/ue5-adventure-packaged.json) · [packaged persistence](evidence/ue5-adventure-packaged-resume.json) · [package/integrity record](evidence/ue5-adventure-package.json) · [audio/art provenance](evidence/ue5-adventure-assets.json).

Actual packaged captures: [title](screenshots/ue5-adventure-packaged-title.png), [gameplay](screenshots/ue5-adventure-packaged-play.png), [completion](screenshots/ue5-adventure-packaged-complete.png). Cover illustration is explicitly labeled and does not represent current 3D model quality.

Use UNREAL_PLAY.cmd or Play Cheweenie Unreal on the desktop. The Windows folder is approximately one GiB and needs no Unreal Editor to play. Source/assets are retained in Git; builds remain ignored. Keep the whole Windows folder together. Mutable Saved data is excluded from payload hashes. First-start driver PSO compilation can hitch; no frame-rate target is claimed.

Build/cook uses installed tools and serial native actions. The verified UE5.8 local shader setting replaces the older XGE-only switch: an initial cook retried a distributed backend before local recovery. A later rebuild hit PCH memory commitment while the playable copy was open; it succeeded after closing only the copy launched for this task. No global memory settings or unrelated apps were changed. Native acceptance and packaging validate this Unreal-only change; Unity tests were not rerun.

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

## Playable Unreal room — subsequent verification

The compiler blocker is resolved. Updating Visual Studio through the existing Microsoft installer supplied MSVC 14.44.35229; the native Development Editor build passed using Windows SDK 10.0.22621.0. An initial accelerated attempt exhausted available Windows memory commitment, so the build now runs one action at a time with XGE/UBA acceleration disabled. A UE5.8 TObjectPtr declaration error was corrected. No compiler ban or engine header was bypassed, no page-file settings were changed, and no unrelated app was closed.

**All sixteen native gameplay checks passed in both headless and rendered DX12 runs.** These exercise the actual Character and Enhanced Input mappings with synthetic keyboard/gamepad events: ground contact, movement/stopping, analog magnitude, hurry press/release, three camera distances, orbit/upward look/recenter, keyboard/gamepad return-home, sustained room-boundary collision and actual movement into the sofa. An initial sofa test failed because stretched sphere collision did not match the visible furniture; runtime boxes now cover the upholstered parts, and the reruns stop Marty at the expected sofa edge while grounded. Explicit Enhanced Input scales avoid inherited legacy camera multipliers. Physical controller hardware remains unverified.

[Native build](evidence/ue5-movement-build.json) · [headless gameplay](evidence/ue5-movement-headless.json) · [rendered gameplay](evidence/ue5-movement-rendered.json) · [actual playable view](screenshots/ue5-marty-movement.png). The rendered run warms the scene for sixty seconds before its checks/capture. Both final gameplay logs contain no Error/Fatal categories. No frame-rate target is claimed.

The native read-only asset audit additionally verifies simple shapes and Pawn/Camera blocking on the floor, rug, sofa seat and back wall. Its wrapper rejects stale output because Unreal's Python executor may exit 0 after a script error; enum compatibility errors were fixed before the final passing audit. The passing asset report is retained above. This is an asset check, separate from the native character collision tests.

Launch LAB_UE5_MARTY.cmd or this machine's Play Cheweenie Unreal desktop shortcut. This is an editor-game prototype requiring the installed Unreal engine, not a standalone package. It retains static pet meshes: realistic walk/Cozy animation, companion behavior, the full family demo loop and neighbourhood port remain future work. Unity and both existing comparison projects' source/assets remain unchanged.

The normal interactive game was opened and reached initialization. Desktop automation returned `GetCursorPos failed: Access is denied (0x80070005)`, preventing an additional OS-level keyboard check. The passing keyboard/gamepad results above are native synthetic events. Snapshot validation retains the new audit wrapper, reports and actual screenshot and excludes generated movement folders; all four movement PowerShell scripts parse.
