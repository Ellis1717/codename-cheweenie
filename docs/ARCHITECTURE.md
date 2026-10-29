# Architecture

Bootstrap creates one owned world hierarchy from an empty scene and guards against duplicate creation. No serialized scene or paid dependency is required. Built-in rendering is the default; URP shader selection is conditional on an active pipeline.

- `CheweenieBootstrap` creates the runtime world and wires components.
- `MartyController` owns Rigidbody movement, grounded jumping, interactions, Cozy and the Between set-piece. `MartyInput` owns legacy input and button maps; explicit DogStickX/Y axes are committed.
- `CanineTuning` and `CanineVisual` separate gameplay tuning and replaceable visuals. Optional Resources assets are MartyTuning and MartyVisual.
- `HouseLayout` defines schematic rooms; `HeadquartersBuilder` creates floors, loft railings, ramp, walls and photo panels. The optional HouseLayout Resources asset replaces the defaults.
- `CatCompanion` owns rest/prowl/inspect/follow states, friendship and optional companion quests. Cat probes exclude their own layer. Obstacle avoidance stops at obstructions; it does not calculate routes.
- `QuestProgress` is pure C# with ordered progression and a required Cozy action at stage 3. `QuestDirector` checks proximity, exposes progress and draws objectives.
- `CozyBlanket`, `BetweenSpot` and `CozyFeedback` supply the blanket visual, cuddle destination, original chime and transient particles.
- `GameAudio` plays synchronized original home/backyard loops and sound cues; `AudioPreferences` migrates and clamps independent category volumes. `OriginalAudio` is the deterministic score/synthesis source used by the offline WAV generator. `OriginalAudioImport` preserves lossless stereo imports. See AUDIO.md.
- `GameSave` stores versioned JSON in PlayerPrefs. `SaveLifecycle` restores quest/friendship at startup and saves on pause/quit. Input binding changes and successful interactions save immediately.
- `CozySettings` pauses gameplay, edits options, captures bindings, swaps conflicting actions, and offers a confirmed New Game. Movement axes remain standard legacy axes.
- `FrontEndMenu` owns the title/start/continue/new-adventure flow, controls/about pages and title pause. `BrandTheme` shares cover artwork, live typography, paw mark and colours with settings. Returning from title settings preserves the title pause; resuming suppresses gameplay input until menu buttons are released.
- `BindingMap` restores unique, device-valid saved controls and swaps conflicts when rebinding. `InputReleaseGate` prevents title/settings button presses from leaking into gameplay. Both are pure C# rules exercised by the standalone validation harness.
- `BackyardProgress` is a separate pure C# discovery/return quest, unlocked after the home mission. `BackyardDirector` owns its HUD objectives, companion dialogue and saves. `BackyardBuilder` creates the schematic stepping stones, ramp/lookout, hedge tunnel and ribbon cache; `BackyardMarker` colours discoveries by state. InteractableTarget sight-line checks prevent collecting through geometry.
- `CheweenieBuild` validates configuration and creates an ignored bootstrap scene for Windows builds. Tests and validation harnesses are described in VALIDATION.md.

## Initial objective loop
1. Greet Lakitu.
2. Greet Rio.
3. Sniff the backyard.
4. Use Cheeween Cozy at the fort.

Companion objectives remain repeatable after main-quest completion so friendship tasks cannot be permanently missed. Save restoration clamps quest/friendship data. Positions and temporary movement states are not saved. The prototype requires licensed Unity runtime validation before release.

After the initial loop, M6 requires three independent backyard landmarks, then the ribbon cache, then greeting Rio. Its additive save field preserves older saves. See BACKYARD.md for route coordinates, save flags and acceptance checks.
