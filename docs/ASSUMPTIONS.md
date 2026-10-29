# Editable assumptions and decisions

This is the catalogue for **Codename .... Cheweenie**. Routine choices are made without interrupting development. Edit a row or add a note when you want a different choice; changes marked **code/build** need to be applied and rebuilt, while **live** settings can be changed in the game. Inferred details are not presented as confirmed facts.

| ID | Current choice | Basis/status | Where to change |
| --- | --- | --- | --- |
| A01 | A single-player Windows family game, windowed at 1280×720 and targeting 60 FPS. | Chosen safe default; other platforms/multiplayer are future scope. | Code/build: CheweenieBuild and CheweenieBootstrap. |
| A02 | Mouse menus, WASD/arrows and Xbox/PlayStation controls use Input System only. | Confirmed requested migration. | Live: Settings bindings/preset. Code: GameControls. |
| A03 | Cozy exploration without combat, moving traffic, purchases or online accounts. | Chosen for a relaxed family demo. | Code/build: game systems and world builders. |
| A04 | Exact home location, raw reference photos and personalized map data stay local. | Privacy-preserving default; public source contains generic support only. | Ignored PrivateReferences and PrivateNeighbourhood Resources; see LOCAL_MAPS.md. |
| A05 | Real street shapes and park/school sites come from OSM; scale is 1.5 and terrain is flat. Home/park/school connectors are simplified game paths. | Mapped geography plus explicitly chosen playability shortcuts; no claim of verified public access. | Local importer/map JSON, then rebuild. |
| A06 | Headquarters rooms, furniture and people remain schematic, not a surveyed reconstruction of the home. | Placeholder until dimensions/art are supplied or confirmed. | Code/build: HouseLayout resource and HeadquartersBuilder. |
| A07 | Lakitu is the mostly white/asymmetric tuxedo cat in the held-cat photo; Rio is the mostly black cat with white bib on the couch. | Inferred from the Keetz tag and earlier references; owner can correct the assignment. | Code/build: bootstrap CatLook assignments and CatVisual. Raw photos stay private. |
| A08 | Cat coats use visible photo markings; uncertain small paw/eye details and proportions are stylized. | Explicit artistic approximation. | Code/build: CatVisual; cover edits use the original private references. |
| A09 | Marty retains the procedural tan Chiweenie and current animation hooks. | Playable placeholder, not a finished likeness or rig. | Code/build: CanineVisual, MartyVisual prefab and MartyTuning resource. |
| A10 | Original synthesized music/effects, default music 35%, effects 60%; existing user preferences win. | Self-contained, no paid/licensed music dependency. | Live: Settings. Code/build: GameAudio and OriginalAudio. |
| A11 | Progress and bindings auto-save locally; returning home never starts a new game. New Adventure requires confirmation when progress exists. | Chosen to preserve family progress. | Live: Settings/New Adventure. Code: GameSave and FrontEndMenu. |
| A12 | Return Marty home is a recovery shortcut, preserving quests, friendship and volume settings. It resumes play and snaps the camera. | Chosen for getting lost/stuck during family play. | Live: pause Settings → Return Marty home. Code: CozySettings. |
| A13 | Bootstrap temporarily suspends existing scene cameras/listeners/directional lights while it owns the view, and restores them on teardown. | Chosen so a default Unity scene is playable without deleting the user's objects. | Code/build: ScenePresentation. |
| A14 | The desktop shortcut and PLAY.cmd use the current tested package; keep older versions closed when playing. | Chosen to reduce version confusion and competing saves. | Local builds/Current and desktop shortcut; source launcher uses relative paths. |
| A15 | Automated device tests use virtual input. Physical controllers, subjective mix and final likeness approval remain human checks. | Honest verification boundary, not a blocker to packaging the playable demo. | Try the current build and record feedback here. |

## Corrections to apply next

Add notes here using the ID above. No corrections are currently pending from the owner.
