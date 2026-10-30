# Codename .... Cheweenie — Cozy Patrol 0.5

A small standalone Windows adventure is now packaged and tested. Run root UNREAL_PLAY.cmd or Play Cheweenie Unreal on the desktop. Find three snacks, visit Lakitu and Rio, then get Cheeween Cozy at the blanket fort. Title/help/pause/sound menus, checkpoint saves, completion, free roam, replay, original audio and the Fox/Leo cameos are included.

Choose Explore the neighbourhood or use the blue OUTSIDE door with E/X-Square. The exterior adds roads, houses, eight editable dog neighbours, park/school destinations and tour-board shortcuts. The local launcher can read the retained private street layout without putting it in public Content. [Neighbourhood and dog names](../../docs/UNREAL_NEIGHBOURHOOD.md).

See [how to play and extend it](../../docs/UNREAL_GAMEPLAY.md) and [Marty's cover-guided model and gait](../../docs/UNREAL_MARTY_ART.md). Marty now uses a separate skin-weighted mesh with 24 joints, grounded procedural leg motion, flexible ears and a close inspection camera. Cats retain their static meshes. The retained Unity release and both comparison projects remain intact; detailed scenery and companion animation remain future work.

## Play

UNREAL_PLAY.cmd prefers builds/UnrealAdventure-0.5.0/Windows/CheweenieUE5Lab.exe, then the preserved 0.4.0 package, then the installed editor-game. The packaged game does not need Unreal Editor. Keep the entire Windows folder together when copying it to another compatible Windows PC. Runtime Saved data is additional and excluded from package hash records. Press V/R3 three times for the close inspection camera; orbit to see ears and knees from the side.

| Action | Keyboard / mouse | Xbox / PlayStation |
|---|---|---|
| Move / look | WASD or arrows / mouse | Left / right stick |
| Hurry | Hold Shift | Hold A / Cross |
| Interact | E | X / Square |
| Camera / recenter | V / R | R3 / RB-R1 |
| Pause | Escape | Menu / Options |
| Return home | H or pause menu | Pause menu |
| Menu choose / confirm / back | Arrows / Enter / Backspace or mouse | D-pad / A-Cross / B-Circle |
| Volume | Left / Right or +/- buttons | D-pad Left / Right |

New Patrol requires confirmation and defaults to Keep. Continue restores mission checkpoints and returns Marty home; sound preferences persist. Test runs use a separate acceptance slot.

## Build and retain resources

Unreal 5.8.4 is installed at D:\Epic Games\UE_5.8. The successful build uses MSVC 14.44.35229 and Windows SDK 10.0.22621.0. No additional compiler installation is needed on this machine. Other development machines need compatible Unreal, C++ tools and a Windows SDK.

    .\BUILD.ps1
    .\IMPORT_AUDIO.ps1
    .\IMPORT_MARTY.ps1
    .\TEST_GAIT.ps1 -Render
    .\TEST_OUTSIDE.ps1
    .\TEST.ps1
    .\TEST_ADVENTURE.ps1
    .\TEST_ADVENTURE.ps1 -Render
    .\PACKAGE.ps1

Scripts accept engine/cache paths. BUILD gathers newly added C++ files and limits concurrency with XGE/UBA acceleration disabled after an earlier memory-commit failure. Shader compilation uses the verified UE5.8 local-only setting. An early cook retried an optional distributed backend before local fallback; future cooks avoid that backend.

Content/Adventure is versioned: the retained cover and six original sounds are included. IMPORT_AUDIO preserves existing Unreal asset edits and records source WAV hashes. BUILD fills a missing generated Content/VisualLab copy from the comparison project without overwriting Adventure. Do not delete the entire Content folder to refresh the comparison copy. Native binaries, caches and builds stay ignored. Original comparison assets/mesh exports remain at their original locations.

PACKAGE builds, cooks, stages and archives under builds/UnrealAdventure-0.5.0. StageOnly reuses an already built/cooked result when updating archive options. The local distribution excludes debug symbols and includes the standard prerequisite installer.

## Actual verification

Version 0.5: native Editor/Game compilation and cook/archive succeeded. The final standalone DX12 executable passed 14 gait + 12 exterior + 25 patrol + six persistence/replay checks. Four additional packaged headless checks passed with the retained private map. Sixteen earlier editor-game movement checks passed at the 60 FPS cap. Final screenshots were inspected after correcting signs, skin surfaces, outdoor lighting and teleport camera framing. [Build report](../../docs/evidence/ue5-05-build.json), [package hashes](../../docs/evidence/ue5-05-package.json), [Marty capture](../../docs/screenshots/ue5-05-marty.png) and [outdoor capture](../../docs/screenshots/ue5-05-park.png). The launcher now uses this validated package; rebuilding invalidates its marker until FINALIZE_PACKAGE.ps1 accepts fresh rendered reports/captures.

Earlier 0.4 baseline, retained for comparison:

The editor-game full patrol passed 25 checks headlessly and in rendered DX12, with cross-process save/reload acceptance. The latest second-process run passed six persistence/replay checks, including confirmed fresh patrol retaining preferences. The original sixteen movement/camera/collision checks still pass.

The packaged Windows executable independently passed all 25 patrol + six persistence/replay checks under DX12. Fresh title/play/completion images were inspected. Final packaged patrol/resume logs contain no Error/Fatal categories. Build/cook/archive succeeded. These are native synthetic-input checks, not physical hardware tests. Muted runs verify audio assets/preferences, not the audible mix.

[Packaged patrol report](../../docs/evidence/ue5-adventure-packaged.json) · [packaged persistence report](../../docs/evidence/ue5-adventure-packaged-resume.json) · [actual title](../../docs/screenshots/ue5-adventure-packaged-title.png) · [actual gameplay](../../docs/screenshots/ue5-adventure-packaged-play.png).

Physical controller/mouse use, subjective mix/likeness and realistic animation remain open. First startup may create driver PSOs and hitch; no frame-rate target is claimed. A prior OS-level keyboard automation attempt returned access denied. No page-file settings or unrelated apps were changed. Unity source/assets/settings remain unchanged and its tests were not rerun for this Unreal-only work.

## Extension points

CozyMissionDefinition Data Assets configure snack/visit IDs, labels, positions, radii, dialogue and sound names. Session commands are Blueprint-callable. CozyAdventure owns progression/saves/audio, CozyHUD owns presentation/hitboxes, and MartyCharacter owns movement/input/cameras. The [extension guide](../../docs/UNREAL_GAMEPLAY.md) explains asset overrides, save versioning and adding mechanics without putting quest rules into the pawn.

Pause freezes player/objective logic while music/UI continue. Future NPCs and timers should respect the session's Playing state. The fort, room proportions and pet scale are provisional schematic choices. The displayed cover is labeled as illustration.
