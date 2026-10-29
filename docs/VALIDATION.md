# Validation and Windows deployment

## Illustrated menus and field guide 0.9.0 — 2026-10-29

Eight title options now lead to ten pages of controls, cameras, menu/recovery help, home/backyard/neighbourhood guidance, illustrated character biographies and About. Actions show live saved bindings. F1 opens help during play; F1/Y/Triangle opens it from paused Settings. Back restores the title, game or originating Settings context. Help pauses and blocks movement, camera/map input; custom F1 gameplay bindings take priority. Three new imagegen illustrations are retained as PNGs with import metadata and full prompts. [Menu guide](MENU_GUIDE.md), [prompts/provenance](MENU_ART_PROMPTS.md).

- **13 final native PlayMode tests passed** on Unity 6000.3.10f1, including a new controller/keyboard guide integration test: sections/tabs, bundled artwork, title pause, Settings origin, blocked movement, direct F1 help and custom F1 priority. Existing controls/physics/art/audio/save/recovery tests remain green. [Native summary](evidence/field-guide-native.json).
- **44 final Windows menu/input/visual checks passed at each of 1280×720 and 1920×1080**, zero runtime errors. Virtual controller browses all ten pages; Input System mouse opens the gallery/selects Rio; back restores title/Settings pause. Actual renders were inspected for layout, text fit and character markings. [720p](evidence/field-guide-menu-720.json), [1080p](evidence/field-guide-menu-1080.json).
- **45 final built-player home/cameo/input checks passed**, zero runtime errors, in **52.54 seconds**. Includes the guide checks, original keyboard/controller movement, camera controls, ramp, cat greetings, home mission completion, both family punchlines and keyboard replay. [Playable report](evidence/field-guide-family.json). Full neighbourhood/save-restart routes were not repeated for this menu/art change; earlier release evidence remains below.
- Runtime/editor/test source compilation has **zero warnings/errors**. Eleven installed package versions and **233 local asset/metadata entries** were audited. A snapshot round trip retains all three new PNGs byte-for-byte and the full prompt document, excluding private reference/map files. Raw family photos remain ignored outside Assets.
- All **139 packaged game files** hash-match the tested final build; the **143-entry ZIP** includes the runtime, updated controls guide and local map/license sidecar. Launch the updated desktop shortcut, `PLAY.cmd` or `builds/FieldGuideWindows-0.9.0/Cheweenie.exe`; archive: `builds/Codename-Cheweenie-Illustrated-Field-Guide-0.9.0.zip`. Close an older game first. Earlier releases are preserved.

Actual renders: [title](screenshots/field-title.png), [actions](screenshots/field-guide-actions.png), [camera help](screenshots/field-guide-camera.png), [Marty](screenshots/field-guide-marty.png), [Lakitu](screenshots/field-guide-lakitu.png), [Rio](screenshots/field-guide-rio.png). New artwork is illustrative menu material; original meshes/coat textures/motion resources and the prior cover remain separate. Physical controller hardware and performance on other systems remain human checks.

## Family cameos 0.8.1 — 2026-10-29

The backyard sniff encounter introduces Fox Ellis as Chief Sniff Inspector: “Yep. Definitely grass.” Cheeween Cozy at the fort introduces Leo Ellis as Blanket Boss: “Five more minutes!” Marty agrees. Each on-screen line lasts eight seconds, including repeat interactions on completed saves. Encounter dialogue stays visible while companion friendship rewards are granted. Existing sound cues remain; these are text cameos.

- **12 native Unity PlayMode tests passed** with the changed source (`TestResults/family-cameos-play.xml`); the Windows build completed successfully on Unity 6000.3.10f1.
- **33 built-player cameo/input/home checks passed**, zero runtime errors, in **45.60 seconds**. Virtual controller input completes the home encounters and verifies both displayed punchlines survive companion completion; keyboard C/E replays both after completion. Existing startup, camera and movement checks run first. [Actual report](evidence/family-cameos.json).
- All **139 packaged game files** hash-match the tested build. The portable ZIP has **143 entries**, including the controls guide and local map/license sidecar. Launch the updated desktop shortcut, `PLAY.cmd` or `builds/FamilyCameosWindows-0.8.1/Cheweenie.exe`; archive: `builds/Codename-Cheweenie-Family-Cameos-0.8.1.zip`. Close an older game first. Previous releases remain available.

This is a focused dialogue/display change. The complete neighbourhood and save/restart results below apply to 0.8.0; those longer routes were not repeated for 0.8.1. Controller verification uses virtual devices.

## Cameras, controller controls and pet motion 0.8.0 — 2026-10-29

Three saved views (Cozy/Explore/Low), R3/V cycling, right-stick/RMB orbit and RB/R1/Home recenter are implemented. Explore/Low movement follows the view. Analog stick magnitude now controls speed on flat ground and ramps. Menus use named A/Cross confirm, B/Circle back and Menu/Options pause; resetting bindings retains the PlayStation preset. GroundedGait drives walk/trot/run timing from actual travel/turning, holds stance paws in world space, probes the floor and adds swing lift, breathing, jump tuck, landing compression and smooth Cozy poses. Cats accelerate/decelerate and turn gradually. Original geometry, root physics, quests and save compatibility remain. [Controls, tuning and limitations](CAMERA_MOTION.md).

- **64 native EditMode and 12 PlayMode tests passed** on Unity 6000.3.10f1 in the isolated project. New tests cover gait timing/profile loading, old-save defaults, controller-only menus/camera controls, paused-look gating, view-relative analog movement, stance contact stability and idle phase. Existing art/collision/quest/save/audio tests remain green. [Native summary](evidence/camera-motion-native.json).
- **145 standalone checks pass**; runtime/editor/test source compilation has zero warnings/errors. Eleven installed package versions and 229 local asset/metadata entries were audited. All three motion profile assets and their metadata are retained.

- **51 final Windows adventure checks passed**, exit 0, zero runtime errors, in **385.28 seconds**, averaging **59.92 FPS at 1280×720** on this machine. The real Rigidbody completes the home missions, both Cozy moves, ribbon delivery, local park/school patrol, walk indoors and recovery using virtual production input. Final states: home 4/backyard 31/local 31. The test returns through the clear hedge passage instead of cutting the cushion corner. [Full report](evidence/camera-motion-adventure.json).
- **32 menu/input/visual checks passed at each of 1280×720 and 1920×1080**, zero runtime errors. Includes R3 presets, right-stick orbit, view-relative walking, RB/R1 recenter, Options pause/Circle back and gait contacts. [720p](evidence/camera-motion-menu-720.json), [1080p](evidence/camera-motion-menu-1080.json).
- **Two save-write and eight save-read checks passed** across separate packaged-player processes, including camera preset/distance, music/effects, missions, friendships, custom keys and PlayStation bindings. Acceptance storage is separate and cleaned afterward. [Write](evidence/camera-motion-save-write.json), [read](evidence/camera-motion-save-read.json).
- All **139 packaged player files** hash-match the final tested build. The **143-entry ZIP** contains the runtime, controls guide, derived local map and ODbL notice. The previous 0.7.0 release is preserved. Launch `PLAY.cmd`, the desktop shortcut or `builds/CameraMotionWindows-0.8.0/Cheweenie.exe`; portable archive: `builds/Codename-Cheweenie-Controller-Cameras-Motion-0.8.0.zip`.
- The [actual Low view](screenshots/camera-low.png) and [24-frame slow-walk recording](screenshots/marty-grounded-walk.gif) were captured from this player and inspected. Recording runs after all mission/recovery checks; private map HUD/street geometry and distant map-dependent props are hidden for the public capture. The normal game keeps the selected neighbourhood.

The supplied models are static meshes with articulated joints. This upgrade adds procedural ground-aware motion; it does not add a fully skinned artist rig or motion-captured anatomy. Physical controllers, subjective mix and performance on other hardware remain human checks. Original family photos/house reference remain local and ignored.

## Retained cats and cozy scenery 0.7.0 — 2026-10-29

Lakitu/Rio now load retained photo-guided prefabs with saved meshes/materials, coat textures, articulated legs/tails and blinking. Lakitu keeps a white front forehead and asymmetric dark eye patch; Rio keeps the dark coat/white chin and bib. No cat mesh was supplied, so these are authored stylizations. Scenery now shares original textured surfaces, rounded furniture, windows, fences, foliage, flowers, cottage roofs and park/school details with warm light/shadows/haze. Existing solid colliders, marker positions and mission routes remain; new props are cosmetic. [Cat guide](../Assets/Art/Cats/README.md), [scenery guide](../Assets/Art/Scenery/README.md).

- **62 native EditMode and 10 PlayMode tests passed** on Unity 6000.3.10f1 in the isolated project. New checks cover retained cat resources/markings, scenery textures/props, geometry budgets, original collision, user scene-object preservation and restoring ambient/fog/shadow settings. Existing controllers, saves, quests, audio and default-scene tests remain green.
- **42 full-adventure checks passed**, exit 0, zero runtime errors, in **375.47 seconds**, with **59.69 average FPS at 1280×720** on this machine. Virtual production input drives the real Rigidbody through all original missions, both Cozy moves, ribbon delivery, the park/school patrol, return indoors and recovery with the new art active. Final states: home 4/backyard 31/local 31. [Full report](evidence/cats-scenery-adventure.json).
- The final Windows player passed **23 menu/input/visual checks at each of 1280×720 and 1920×1080**, exit 0, zero runtime errors. [720p](evidence/cats-scenery-menu.json), [1080p](evidence/cats-scenery-menu-1080.json). Actual [Lakitu](screenshots/lakitu-portrait.png), [Rio](screenshots/rio-portrait.png), [interior](screenshots/living-room-art.png), [headquarters](screenshots/headquarters-art.png) and [backyard](screenshots/backyard-art.png) renders were inspected. Public showcase captures hide private street geometry and distant map-dependent props, with a neutral temporary ground backdrop; normal gameplay uses the selected neighbourhood.
- **Two save-write and eight save-read checks passed** across separate packaged-player processes, restoring all missions, friendships, bindings, preset, volumes and camera distance. Acceptance storage is separate and cleaned afterward. [Write](evidence/cats-scenery-save-write.json), [read](evidence/cats-scenery-save-read.json).
- **145 standalone checks pass**; runtime/editor/test compilation has zero warnings/errors. Eleven package versions and 220 local asset/metadata entries were audited. All **139 packaged player files** hash-match the tested build. The **143-entry ZIP** includes runtime, start guide, derived local map and ODbL notice; raw photos are excluded.

Launch the updated desktop shortcut, `PLAY.cmd` or `builds/CatsSceneryWindows-0.7.0/Cheweenie.exe`. Portable archive: `builds/Codename-Cheweenie-Cat-Crew-Scenery-0.7.0.zip`. Close an older game first. In an already-open Unity Play session, stop and restart Play to load the new prefabs. The older Current folder can be preserved if in use. Physical controllers, subjective mix, final likeness and performance on other hardware remain human checks. House layout and people remain schematic.

## Retained original Marty Explorer 0.6.1 — 2026-10-29

The default now uses the owner's supplied `Marty_Explorer.obj`, with its original 7,666 vertices, 15,136 triangles and ten colours. Both original OBJ/GLB are retained byte-for-byte under Assets/Art/Marty/Source. The Unity importer fits the mesh uniformly to the gameplay root and adds simple joints to disconnected surfaces. The supplied model is static, with no skin/animation clips/fur textures. The more detailed concept illustration is an art target, not the same geometry. A photo-guided alternative is also retained; [resource guide](../Assets/Art/Marty/README.md).

- **59 native EditMode and 8 PlayMode tests pass** on Unity 6000.3.10f1. New checks verify original source SHA-256, GLB signature, source colours and exact vertex/triangle preservation, finite geometry/budgets, joint structure, loading, animation and Cozy pose. The original single Rigidbody/capsule and existing virtual keyboard/Xbox/PlayStation, quests, saves and audio tests remain covered.
- The final Windows build passes **20 startup/menu/input/visual checks**, exit 0, zero runtime errors, including loading the saved character and retaining gameplay physics. The [actual Explorer player render](screenshots/marty-portrait.png) was inspected; [menu report](evidence/marty-character-menu.json). The separately retained [photo-guided alternative render](screenshots/marty-photo-alternative.png) is provided for comparison.
- **145 standalone checks pass**; runtime/editor/test compilation has zero warnings/errors. Eleven packages and 117 local metadata entries are audited (115 public, two ignored map entries).
- Snapshot export retains GLB as Base64. A 289-entry export round trip preserves the original GLB bytes and excludes private map/reference files. Original photos, house reference and full concept sheet stay local and ignored.
- The final Explorer executable passes **39 full-adventure checks**, exit 0, zero runtime errors: original quests, both Cozy moves, ribbon delivery, the full park/school patrol, return indoors and home recovery. Final states are home 4/backyard 31/local 31. Run: **377.01 seconds, 59.70 average FPS at 1280×720** on this machine. [Full report](evidence/marty-character-adventure.json).
- The packaged executable passes **2 save-write and 8 save-read checks** across separate processes, restoring missions, friendships, bindings, controller preset, volumes and camera settings. Acceptance storage is separate and removed afterward. [Write](evidence/marty-character-save-write.json), [read](evidence/marty-character-save-read.json).
- It also passes **20 menu/input/visual checks at 1920×1080**, exit 0, zero runtime errors. [1080p report](evidence/marty-character-menu-1080.json). Native suites were repeated with isolated company/product preferences: 59 EditMode and 8 PlayMode passes.
- All **139 packaged player files** hash-match the tested build. The **143-entry ZIP** includes runtime, start guide, private derived map and ODbL notice, with no raw reference photos. The previous Current release is preserved locally as FamilyWindows-0.5.0. Current and the desktop launcher now use Explorer 0.6.1.

Launch `PLAY.cmd` or `builds/Current/Cheweenie.exe`; portable archive: `builds/Codename-Cheweenie-Marty-Explorer-0.6.1.zip`. In an already-open Unity Play session, stop and restart Play to instantiate the new resource prefab.

The retained source/default/alternative prefabs, Unity mesh/material assets, metadata, importer, generator and animation code are committed together. Normal builds preserve an existing custom default and material edits. Character motion is an articulated hierarchy rather than a skinned FBX; owner review remains the final likeness check. House/people geometry is still schematic. Physical hardware, subjective mix and performance on other machines remain human checks.

## Editor TagManager import fix — 2026-10-29

The source TagManager stored unnamed layers as bare `-` lines. Unity 6.3.10f1 rejected that file at the following sorting-layer section (line 41). The earlier isolated project had been normalized by Unity, masking the source formatting problem. Unnamed source layers now use explicit empty strings; all 32 layer positions and the default sorting/rendering layers are retained.

The corrected source file was copied into the isolated project before native validation: **56 EditMode and 7 PlayMode tests passed**, with no settings-parser or C# compilation errors in either log. Local evidence: `TestResults/tagmanager-edit.xml/.log` and `tagmanager-play.xml/.log`. `Validation/ManifestChecks.ps1` rejects the original bare-marker form; its guard was checked against both the original Git content and corrected source. Package/metadata validation also passes. Runtime source and the previously tested Windows package are unchanged.

In the main editor, allow refresh, clear old Console messages and press Play. If it retains the earlier failed settings import, save the scene and restart Unity once. The bootstrap generates the world during Play; an empty Game view before Play is expected.

## Family release 0.5.0 — 2026-10-29

Launch **Play Cheweenie** on the desktop, `PLAY.cmd`, or `builds/Current/Cheweenie.exe` after closing older copies. Unity is not required. The portable archive is `builds/Codename-Cheweenie-Family-0.5.0.zip`; extract the whole archive. The sections below this release are historical results and their older launch paths are superseded by Current.

This release adds **Return Marty home** to gameplay Settings. It resumes play and snaps Marty/camera home while preserving quests, friendship, bindings and volume. Bootstrap now suspends pre-existing scene cameras/audio listeners/directional lights, then restores them when its world is destroyed. This supports the default Unity scene without duplicate presentation components or deleting the user's objects. The build requests a single game instance. [Editable assumption catalogue](ASSUMPTIONS.md) records chosen defaults, inferred photo assignment and remaining art placeholders.

- **56 native Unity EditMode tests and 7 PlayMode tests passed** on Unity 6000.3.10f1. New tests cover default-scene presentation suspension/restoration and home recovery without progression/settings loss. Existing quests, Input System devices, audio and cat visuals remain covered.
- **145 standalone checks passed**. Runtime/editor/test compilation passed with zero warnings/errors after restoring the harness projects. An initial `--no-restore` invocation encountered the shared harness cache's different target framework; restoring each project resolved that tooling-only failure. Eleven package versions and 83 local metadata entries were audited (81 public, two private).
- The rebuilt Windows player passes **18 startup/menu/input/visual checks**, exit 0, with zero runtime errors. This includes the single active camera/listener, scaled mouse buttons, keyboard movement/jump, PlayStation Cross, both cat textures and map controls. [Menu result](evidence/family-menu.json).
- **38 full-adventure checks passed**, exit 0, zero runtime errors, in **375.47 seconds** at **59.93 average FPS** at 1280×720 on this machine. Production virtual input drives the real Rigidbody through headquarters ramps, both cats, both Cozy moves, the backyard ribbon mission, all four mapped park/school stops and Home. The new pause-menu mouse recovery and camera snap also pass. Final progression is home 4, backyard 31, local patrol 31. [Full report](evidence/family-adventure.json).
- **Two save-write and eight save-read checks passed across separate player processes** using the packaged executable. All mission flags, both friendships, companion completion, custom keyboard binding, PlayStation preset, music/effects levels and camera distance restore; Continue preserves them. The test uses a separate acceptance save key and removes it afterward. [Write](evidence/family-save-write.json), [relaunch/read](evidence/family-save-read.json).
- The packaged executable also passes **18 checks at 1920×1080**, exit 0, zero runtime errors, confirming scaled mouse/menu interaction and cat renders at the larger viewport. [1080p report](evidence/family-menu-1080.json).
- All **139 player files** in Current match the tested build by SHA-256. The **143-entry portable ZIP** includes the full runtime, start guide, derived local map and ODbL notice; raw reference photos are excluded. Builds and private data remain ignored. The desktop shortcut and relative root launcher target Current.

Release checks use virtual input through production Input System code. They do not establish physical controller compatibility on every driver, subjective speaker mix quality, final likeness approval or performance on other hardware. Original reference photos and personalized map screenshots remain local and ignored. Normal game saves are separate from opt-in acceptance saves.

## Photo-guided cat release 0.4.0 — 2026-10-29

The original Lakitu/Rio photos were recovered from the earlier game chat and used to correct the cover and runtime cat visuals. Lakitu has the mostly white face, asymmetric black eye patch/blaze, white chest/paws, pink nose and collar; Rio has the mostly black face/body, small white chin and chest bib. [Marking notes and provenance](BRANDING.md). Raw reference photos remain outside Unity in ignored PrivateReferences; they and their household/person backgrounds are not bundled or published.

- **56 native EditMode tests and 5 PlayMode tests passed**, including asymmetric coat/white-chin rules, distinct profiles, generated face texture/shader availability, four paws, one original kinematic Rigidbody per cat and disabled cosmetic colliders. Existing quests, audio and input tests remain green.
- The rebuilt Windows player passed **17 startup/input/visual checks**, exit 0, zero runtime errors. Mouse settings/start, keyboard movement/jump, controller mapping, map controls and both generated cat faces pass. Actual player screenshots were captured face-on after temporarily disabling Rigidbody interpolation for the capture pose. The private street layer and gameplay HUD are hidden in the shareable portraits: [Lakitu](screenshots/lakitu-portrait.png), [Rio](screenshots/rio-portrait.png), [title](screenshots/title.png), [settings](screenshots/settings.png). [Acceptance report](evidence/cat-reference-acceptance.json).
- These checks establish working visuals/physics, not an exact likeness score. The cover and procedural cats remain stylized approximations. The earlier long neighbourhood walk was not repeated for this cosmetic update; native quest smoke tests still pass.
- All 145 existing standalone checks passed, runtime/editor/test compilation had zero warnings/errors, and 11 package versions plus 81 local asset/metadata entries were audited (79 public, two private). All 139 distributed player files match the tested build; ZIP/runtime/map/licence checks pass, and original reference photos are excluded. Input System only, private neighbourhood data, music and saved progression are preserved.

Launch `builds/CatReferenceWindows-0.4.0/Cheweenie.exe`, extract `builds/Codename-Cheweenie-Photo-Cats-0.4.0.zip`, or use the updated desktop Play Cheweenie shortcut after closing an older copy. Original reference photographs are not included in either distribution.
## Input System only release 0.3.1 — 2026-10-29

The project now uses **Input System Package (New)** only (`activeInputHandler: 1`). Input System 1.17.0 was already installed; all remaining Input Manager reads are removed. Unknown axes/absent devices return neutral input. Supported saved KeyCode bindings remain compatible, including PrintScreen/context-menu aliases; unsupported old character bindings restore to usable defaults. The Windows configuration validator requires the new backend and no longer depends on legacy axes.

IMGUI is retained for drawing, with mouse hit-testing/presses now supplied explicitly by Input System. Click positions account for the GUI scale, and a press is consumed once even across multiple GUI passes. Keyboard and controller menu navigation remain supported.

The new-only Windows player passed **14 startup/input/menu acceptance checks**, exit 0, zero runtime errors. Actual virtual mouse presses open Settings, adjust music, return to the paused title and start the adventure; keyboard walking/jumping, PlayStation Cross and map open/close also pass. [Report](evidence/input-system-only-acceptance.json). The long park/school walk was verified in release 0.3.0 below and was not repeated for this input-focused change.

**53 native EditMode and 4 PlayMode tests passed** with the legacy backend disabled. Coverage includes input movement/actions, scaled mouse coordinates, one-click consumption, Print bindings and keyboard/gamepad removal while the legacy backend is disabled. Runtime/editor/test compilation passed with zero warnings/errors, and all 145 existing standalone checks passed. No Input Manager deprecation warning appeared in the new-only editor/build/player logs. All 139 packaged player files match the input-tested executable. Physical controller hardware and subjective speaker mix remain human checks.

Save any unsaved scene changes and **restart the main Unity editor** after changing backends; its existing session was left open. Then press Play to bootstrap the world from the scene. An empty Game view before Play is expected. Old Console warnings may remain as historical entries. The desktop Play Cheweenie shortcut targets `builds/LocalMapWindows-0.3.1/Cheweenie.exe`; the portable archive is `builds/Codename-Cheweenie-Local-Neighbourhood-0.3.1.zip`.
## Private local-map release 0.3.0 — 2026-10-29

- **53 native Unity EditMode tests and 4 PlayMode tests passed**. New coverage validates the map schema, relative coordinates, invalid roads/stops, school-site reach, inherited JSON fields and migrating a three-stamp patrol into the school extension.
- The full Windows walk passed **30 acceptance checks**, exit 0, in **370.83 seconds**, with **zero runtime errors** and **59.92 average FPS** at 1280x720. Production virtual keyboard/gamepad input completes the original quests and both Cozy moves, opens/closes the map, visits all four mapped stops, returns Home and walks back inside. Final states: home 4, backyard 31, local patrol 31. [Full-walk report](evidence/local-map-acceptance.json).
- A subsequent optional-school schema fix and test-helper refactor were followed by native validation and a rebuild. The final distributed executable passed a separate **11-check startup/menu/input smoke**, exit 0, zero runtime errors, including loading its private map and school exterior. It checks UI/startup rather than repeating the long walk. [Final menu smoke](evidence/local-map-menu-smoke.json).
- **145 existing standalone checks passed**; runtime/editor/test compilation passed with zero warnings/errors. Eleven package versions and 78 local asset/metadata entries were audited, including the two ignored private-map entries. The public source has 76 entries without those private resources.

The local map has 92 exported road/path segments, two mapped green-space areas and a school stop at the mapped school site. Streets retain their geometry with uniform 1.5 scale. Terrain, buildings, home access and short park/school walking connectors are simplified for play. The map is not a survey or a verified public-access route.

Launch `builds/LocalMapWindows/Cheweenie.exe` or extract `builds/Codename-Cheweenie-Local-Neighbourhood-0.3.0.zip`. Quit an older running game first. The desktop Play Cheweenie shortcut points to this edition. Exit through the south garden gate; use M / View / Share for the expanded map. Original missions remain playable.

Private map/geocode caches, home-relative JSON and personalized screenshots remain ignored locally. The family ZIP includes its derived JSON and ODbL notice under MapData; the public PR includes only the generic loader/importer, synthetic tests and reports without addresses or coordinates. See [local map workflow and attribution](LOCAL_MAPS.md). Physical controllers and subjective speaker mix remain human checks.
## Neighbourhood release 0.2.0 — 2026-10-29

- **46 native Unity EditMode tests and 4 PlayMode tests passed**. New coverage includes all six landmark orders, old-save compatibility, JSON serialization, independent progression, gate/ground geometry, actual quest routing and New Adventure reset.
- The release Windows executable passed **26 acceptance checks**, exit 0, with **zero runtime errors**. Production virtual keyboard/gamepad input walks the complete home/backyard adventures, both Cozy moves, the east garden gate, mailbox, flower garden, park, welcome-home sign and return indoors. Final flags: home 4, backyard 31, neighbourhood 15.
- Final run: **126.58 seconds, 59.80 average FPS at 1280x720**, capped at 60 FPS on this development machine. This is not a cross-hardware benchmark. See [JSON result](evidence/neighbourhood-acceptance.json), [garden screenshot](screenshots/neighbourhood-garden.png) and [park screenshot](screenshots/neighbourhood-park.png).
- **145 existing standalone checks passed**; runtime/editor/test compilation passed with zero warnings/errors. Eleven package versions and 73 metadata entries validated. No additional dependency was needed for the neighbourhood.
- All 139 distributed player files match the tested build. The portable ZIP has 142 entries including the complete runtime and a start guide. The old local release was preserved.

Launch `builds/NeighbourhoodWindows/Cheweenie.exe`, or extract `builds/Codename-Cheweenie-Neighbourhood-0.2.0.zip`. The desktop **Play Cheweenie** shortcut points to this edition. Quit any older running copy before starting the new one so two versions do not overwrite each other's saves. The neighbourhood is available immediately through the east garden gate. Its schematic geography is fictional until family landmarks are supplied. Physical controllers and subjective speaker mix remain human checks.
## Earlier Windows/audio results — 2026-10-28

Unity 6000.3.10f1 now imports and tests this project successfully. Audio validation ran in an isolated copy at `Validation/UnityAudioProject`, with a different company/product name to keep its PlayerPrefs separate from the user's game. The user's running editor was preserved. The repository now includes reproducible Unity 6.3.10f1 settings, a package lock and Both input handling.

- **30 Unity EditMode tests passed**, zero failures. Includes quest progression, backyard rules, bindings and imported audio assets/volume migration.
- **3 Unity PlayMode tests passed**, zero failures. The world smoke test checks title/settings transitions, floor settling, home and backyard quest interactions, ribbon delivery and New Adventure reset. The virtual-device test exercises production keyboard/Xbox/PlayStation input and the Cozy-before-movement regression. The audio test checks scheduled music playback/time advancing on the paused title, menu audio, separate volume controls and home/backyard crossfades.
- **145 standalone checks passed**: 13 home quest, 23 bindings/input gate, 76 backyard and 33 audio. Audio checks cover volume migration/mute, PCM sample safety, loop duration/stereo content and loop-boundary continuity.
- Runtime/editor/test source compilation against installed Unity 6.3 assemblies targeting .NET Standard 2.1 passed with zero warnings/errors.
- Eleven manifest package versions match the bundled editor packages. Asset metadata and assembly-definition JSON are audited by `Validation/ManifestChecks.ps1`.
- Eleven original stereo WAV files were rendered: two forty-second music loops and nine short cues, approximately 7.6 MB total. Generator validates finite, non-clipped, non-silent samples.

The first actual PlayMode run exposed a test setup issue: an interpolated Rigidbody teleport had not updated the rendered Transform before immediate interaction checks. The test now synchronizes both poses. The corrected world smoke and the added audio playback test pass.

Headless tests do **not** verify speaker output, subjective mix quality, rendered menu layout, physical keyboard/controller input or complete manual traversal. The world smoke uses synchronized teleports for quest interactions. A release Windows player and rendered 1280x720 screenshots are now produced separately from the headless test runner.

## Earlier attempts

Before the user's Unity activation/session, batch startup exited 198 because no valid editor license was available. Those attempts did not import or test the project. The successful audio-era results above supersede that blocker.

## Play in the editor

1. Open the project in Unity 6.3 LTS. Let imports and compilation finish.
2. Set **Active Input Handling** to **Input System Package (New)** if necessary; restart when prompted. Run **Cheweenie > Validate project configuration**.
3. Stop any existing Play session before testing changed scripts. Use an empty scene, then press Play and click the Game view.
4. The title should display the cover and play the home theme even though gameplay is paused. Ensure the Game view's Mute Audio control is off.
5. Open Settings. Adjust **Music** and **Sound effects** separately with Left/Right, horizontal stick input, or clicking/activating the row. Music should remain audible when effects are muted; menu clicks and effects should work when music is muted. Closing title settings must leave gameplay paused.
6. Start/Continue. Walk/sprint/jump and listen for soft footsteps, jump and landing cues. Greet the cats; verify distinct stylized chirps. Confirm a single Cozy flourish and Between reward sound.
7. Walk into the backyard: music should fade smoothly to the brighter loop. Return indoors or to title: home music returns. Opening settings softens music; Cozy softens it further. Listen through the forty-second loop boundary for clicks.
8. Finish the original home quest and the backyard adventure in `docs/BACKYARD.md`. Verify walls block interactions. Walk the house/yard ramps, cross loft seams, jump the stones and walk both directions through the hedge tunnel.
9. Test How to Play/About/Back, New Adventure confirmation, Return to Title, and menu layouts at 1920x1080, 1280x720 and 4:3. Verify no menu button leaks into a gameplay action.
10. With Xbox and PlayStation controllers, test all actions and remapping. Driver mappings vary; choose a preset, then rebind as needed. Standard left-stick axes have a 0.2 dead zone; axis rebinding is not implemented.
11. Restart Play and check that progress, friendship, bindings and both volume levels restore. New Adventure must reset world progression/spawns and preserve settings.
12. Check the Console and record any red error with its stack trace. Capture screenshots only from a real play session.

## Automated tests

In **Window > General > Test Runner**, run **EditMode > Run All**, then **PlayMode > Run All**.

Successful headless commands used the editor below with `-batchmode -nographics -projectPath "<isolated-copy>" -runTests`, `-testPlatform EditMode` or `PlayMode`, and explicit `-testResults`/`-logFile` paths. No `-quit` flag is added to test runs; the runner exits itself.

Editor: `C:\Program Files\Unity\Hub\Editor\6000.3.10f1\Editor\Unity.exe`

Local evidence (ignored by Git):
- `TestResults/release-edit.xml`, `TestResults/release-edit.log`
- `TestResults/release-play.xml`, `TestResults/release-play.log`

Standalone commands from the project root:
```powershell
dotnet run --project Validation/QuestChecks.csproj
dotnet build Validation/Compile.csproj
dotnet build Validation/UnityTestsCompile.csproj
```

For a different installation, pass `-p:UnityEditorPath='C:\path\to\Editor'`. The optional UnityTestsCompile harness references a bundled 3D template TestRunner assembly; adjust its path if your installation differs.

## Windows build

Windows Build Support is installed on the development machine. Save all open scenes, then choose **Cheweenie > Build Windows game**. The utility validates input and Windows Build Support configuration, sets the exact product title, creates an ignored bootstrap scene under `Assets/Generated`, restores the editor scene setup and builds `builds/Windows/Cheweenie.exe`. It refuses unsaved scenes/changes.

Distribute the entire Windows output folder, including its Data directory and UnityPlayer DLL. Keep build output out of Git. The build uses Mono and LZ4HC compression. Resources materials explicitly reference world/particle shaders so the player cannot strip dynamically requested shaders. See the built-player acceptance section below.

## Built-player acceptance

The Windows executable supports an explicit opt-in test mode:

```powershell
& '.\builds\Windows\Cheweenie.exe' -cheweenie-self-test -cheweenie-report "$PWD\TestResults\WindowsAcceptance\result.json" -logFile "$PWD\TestResults\WindowsAcceptance\player.log"
```

This creates virtual keyboard/gamepad devices inside the game process, drives the actual Rigidbody through the route, writes JSON and screenshots, then exits 0 on success or 2 on failure. It uses isolated save reads and disables persistence, preserving real player progress. Normal launches do not create virtual devices. Run with a visible window so frame presentation/screenshots are enabled.

Acceptance covers the branded title/settings, keyboard walking/jumping, PlayStation Cross mapping, headquarters ramp ascent/descent, both cats, home quest, Cheeween Cozy and Cheween in Between, backyard landmarks/ribbon delivery, and Return to Title. This is automated device input, not physical hardware testing. **Final result: PASS**, exit 0, 19 checks, 75.34 seconds, 59.68 average FPS at 1280x720 on this development machine. Home stage 4 and backyard flags 31 confirm the full mission completed; runtime errors: zero. The game targets 60 FPS. This is a single-machine capped run, not a cross-hardware performance benchmark. See [JSON report](evidence/windows-acceptance.json), [title](screenshots/title.png), [settings](screenshots/settings.png) and [completed adventure](screenshots/complete.png).

## Play the portable release

Open `builds/Windows/Cheweenie.exe`. Unity does not need to be running. Choose **Start adventure**, use WASD to move, Space to jump, E to greet/sniff, C for Cozy and Escape for settings. Greet Lakitu, greet Rio, sniff the backyard marker, then rest at the blanket fort. The backyard ribbon adventure unlocks afterward. Music and sound effects have separate volume controls.

For another Windows PC, extract `builds/Codename-Cheweenie-Windows-0.1.0.zip` completely before launching. Keep the executable, Data folder, UnityPlayer DLL and Mono runtime together. The current art and house geometry remain prototype placeholders; the title cover is concept art rather than an in-game model preview.
