# Codename .... Cheweenie — Cozy Patrol

Version 0.8 adds [maps, location/direction finding and saved feature controls](UNREAL_NAVIGATION.md). M / D-pad Up opens destinations; Pause → Controls & cameras offers 24 switches. Version 0.7 adds [health, backpack, tricks and alternative body controls](UNREAL_MARTY_07.md). Optional features default on and can be changed without resetting the patrol.

Version 0.6.1 adds B / LB-L1 barking, action effects and location music; see [the audio guide](UNREAL_AUDIO.md).

Version 0.6 adds [Marty's energy and pet-action sandbox](UNREAL_DOG_LIFE.md): digging, carrying/burying/recovering toys, jumps and sofa hops, TV rest and kitchen food. These remain available after completing the patrol. Space/Y-Triangle jumps, F/RT-R2 digs and Q/LT-L2 drops; E/X-Square uses nearby objects.

The Unreal version has a small complete adventure loop: title → patrol → three snacks → Lakitu → Rio → blanket fort → Cheeween Cozy → free roam or replay. Version 0.5 adds a cover-guided skin-weighted Marty, procedural canine leg/ear movement and a fourth close inspection camera. The cats retain their previous static models. See [Marty art and movement](UNREAL_MARTY_ART.md) for source, tuning and limitations. This is a foundation for more gameplay, not the full Unity adventure or a finished photorealistic art pass.

## Play

Run root UNREAL_PLAY.cmd. It prefers the validated packaged Windows 0.6.1 build, then preserved 0.6.0 and 0.5.0 and 0.4.0 packages, then opens the installed Unreal editor-game. Play Cheweenie Unreal on the desktop is the convenient entry point on this machine.

Choose Continue / start patrol. Golden snacks collect when Marty walks close enough. After finding all three, follow the destination label/distance, approach Lakitu and then Rio, and press Interact. Visit the blue blanket fort last. The completion menu offers free roam or another patrol. Fox Ellis is the Chief Sniff Inspector; Leo Ellis is the Blanket Boss.

| Action | Keyboard / mouse | Xbox / PlayStation |
|---|---|---|
| Move | WASD or arrows | Left stick |
| Look | Mouse | Right stick |
| Hurry | Hold Shift | Hold A / Cross |
| Interact | E | X / Square |
| Cycle camera | V | R3 |
| Recenter | R | RB / R1 |
| Pause / resume | Escape | Menu / Options |
| Return home | H or pause menu | Pause menu → Return Marty home |
| Select menu row | Up / Down or click | D-pad Up / Down |
| Confirm | Enter or click | A / Cross |
| Back | Backspace / Escape | B / Circle |
| Change volume | Left / Right or +/- buttons | D-pad Left / Right |

New Patrol requires confirmation and initially selects Keep. Continue restores checkpoint progress and returns Marty home, rather than saving exact avatar position. Progress and volume preferences save automatically. The native slot is CheweenieAdventureV1, independent of Unity saves. Development/editor and packaged builds may use different Saved directories; no automatic migration between those locations is claimed.

Pause blocks player movement and objective collection; music and UI continue. There are no moving NPC systems yet. Future AI/timers should respect the session's Playing state too; this prototype does not globally pause Unreal's world.

## Extend without rewriting the controller

| Source | Responsibility |
|---|---|
| CozyAdventure.h / .cpp | Mission definition, progression, session/menu state, save schema, audio, snack markers and temporary room/fort dressing |
| CozyHUD.cpp | Canvas menu/prompt presentation and mouse hitboxes |
| MartyCharacter.h / .cpp | Movement, Enhanced Input, cameras, action forwarding and base room collision |
| AdventureSmoke.cpp | Opt-in native traversal/menu/save acceptance; separate test slot |
| MovementSmoke.cpp | Original movement/camera/collision regression acceptance |
| Content/Adventure | Versioned game-owned Unreal assets |
| Content/VisualLab | Generated copy of the independently retained comparison assets |

In Unreal Content Browser, create a Data Asset → CozyMissionDefinition under /Game/Adventure/Missions. Edit MissionId, Title, Snacks and Visits. Each stop exposes ID, label, position, interaction radius, dialogue and sound name. All snacks are collected first; visits then run in array order and the last visit completes the mission. This is a small ordered-patrol model, not an arbitrary branching quest graph.

Point the project to your asset in Unreal/UE5MovementLab/Config/DefaultGame.ini:

    [Cheweenie.Adventure]
    MissionAsset=/Game/Adventure/Missions/MyPatrol.MyPatrol

Change MissionId when an edited definition should reset old progress. Keep snack count between 1 and 20 and include at least one visit. Place destinations on reachable floors within the current room; expanding the world also requires updating its physical bounds. Default acceptance deliberately uses the default mission, independent of this override.

Session commands (Start, Confirm, Back, Pause, Navigate, Adjust, Interact and Save) are Blueprint-callable. GameMode.Adventure exposes session state. New UI can call those commands instead of changing movement code. New mechanics should have their own Actor/ActorComponent and update mission state through a clear interface.

Keep new imported meshes, textures, audio, maps and mission assets under Content/Adventure, which is retained in Git and snapshots. Preserve original source files and license/provenance alongside them, normally under SourceAssets. The inherited Content/VisualLab/Python preview copies and caches/build products remain ignored. Duplicate a comparison asset into Adventure before editing it. The base-copy step fills a missing VisualLab folder without overwriting authored Adventure content.

Sound names resolve under /Game/Adventure/Audio/Name.Name. Six retained original WAVs are imported by IMPORT_AUDIO.ps1; Home loops. The retained cover is imported under /Game/Adventure/Art/Cover. Normal imports preserve existing Unreal edits. Additional original sounds can be imported into the same Audio folder and named in mission stops. Raw Unity resources are read-only inputs; they were not changed.

## Build and verify

From Unreal/UE5MovementLab:

    .\BUILD.ps1
    .\IMPORT_AUDIO.ps1
    .\TEST.ps1
    .\TEST_ADVENTURE.ps1
    .\TEST_ADVENTURE.ps1 -Render
    .\PACKAGE.ps1

BUILD uses installed Unreal 5.8.4, MSVC 14.44+ and a Windows SDK. Serial compilation with acceleration disabled reduces memory commitment. Source-file gathering allows newly added C++ files to enter the build. Engine/cache locations are parameters in the scripts. Current package output is ignored under builds/UnrealAdventure-0.6.1; 0.6.0, 0.5.0 and 0.4.0 remain the archived fallback.

Close the running copy before overwriting its package directory, or provide a different OutputRoot. Runtime Saved folders contain preferences, progress, crash-handler configuration and caches; keep those separate when preparing a clean distribution. Integrity records hash the immutable payload rather than mutable Saved data.

TEST_ADVENTURE accepts -GameExe for the inner packaged executable and -OutputRoot for reports/captures. It drives the real pawn using synthetic input, walks the full patrol, checks menus/settings/restart safety, and reopens a second process to verify persistence. Only the named CheweenieAdventureAcceptance test slot is cleared; the player's slot is never used. Fresh reports and images are required. Muted automated runs check sound assets/preferences, not audible mix or physical hardware.

Actual results, captures and packaging status are recorded in the graphics-lab guide and evidence files. Physical controller/mouse checks, subjective audio/likeness, detailed animation and measured performance remain separate human/art checks.
