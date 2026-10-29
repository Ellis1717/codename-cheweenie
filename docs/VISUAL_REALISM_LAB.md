# Separate graphics experiment

This branch is an experiment alongside the stable **Codename .... Cheweenie** 0.9.0 game. It is not a replacement release. The original checkout, desktop Play Cheweenie shortcut, current build, models and saves are preserved. The branch has not been merged into main.

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

UE5 was not found installed during this audit. The 4.27 spike does not exercise UE5 Lumen/Nanite and cannot decide their final quality/performance. An eventual UE5 prototype should be another project copy, beginning with one rigged pet and one finished room. Keep the stable Unity release while comparing actual moving gameplay at the intended resolution.

The installed Unreal editor successfully rebuilt/saved a 12-actor room with exactly three static pet actors, and a standalone editor-game launch produced an [actual material-ready render](screenshots/unreal-comparison.png). Early captures showed compiling placeholder materials and overexposure; the fill light was reduced. Shader compilation and lighting remain a rough benchmark setup, not a finished art scene. Unreal gameplay/controller interaction was not acceptance-tested.

Unreal can help with indirect lighting/reflections, but the same simplified mesh remains simplified in another engine. The main next investment is character/environment asset quality. Engine choice should follow that controlled prototype rather than a full rewrite first.

Primary references: [Epic: Lumen](https://dev.epicgames.com/documentation/en-us/unreal-engine/lumen-technical-details-in-unreal-engine), [Epic: hardware requirements](https://dev.epicgames.com/documentation/en-us/unreal-engine/hardware-and-software-specifications-for-unreal-engine), [Epic: editor Python](https://dev.epicgames.com/documentation/en-us/unreal-engine/scripting-the-editor-using-python?application_version=4.27), [Unity: Built-In surface shaders](https://docs.unity.com/en-us/engine/6000.3/manual/materials-and-shaders/shaders/writing-shaders-birp/writing-surface/sl-surface).
