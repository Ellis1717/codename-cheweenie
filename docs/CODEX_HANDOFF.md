# Codex remote handoff

Repository: https://github.com/Ellis1717/codename-cheweenie
Default branch: main

## Remote Codex starter prompt
Work on Codename .... Cheweenie. Read AGENTS.md, README.md, docs/ARCHITECTURE.md and all scripts. First ensure Unity 6.3 LTS opens and compiles the prototype. Fix any issues, then execute milestones M1–M5 below in successive focused commits/PRs. Preserve a playable state at each stage; report tests actually run and remaining blockers. Use placeholder assets if needed. Do not silently replace user-specific house details with invented facts.

## Milestones
- **M1**: Validate Unity compatibility, scene bootstrap, input, collision, quest progression, editor checks; add Unity Test Framework edit-mode tests for pure quest logic.
- **M2**: Replace Marty primitive with configurable canine character controller, animation hooks, character tuning, and import-ready custom 3D dog mesh pipeline.
- **M3**: Expand house headquarters into modular two-story room builder with editable dimensions, room definitions and photo reference placement; don't assume exact floor plan.
- **M4**: Cat NPC state machines and distinct behavior: Lakitu / Keeterman energetic, Rio / Sis / Miss Priss opinionated, friendship interactions and companion quests.
- **M5**: Cozy mechanic polish (blanket hideaway), Cheween in Between set-piece, audio/particles, save data, settings menu, controller rebinding support.
- **M6**: World expansion: backyard traversal, secret routes, dialogue and additional missions.
- **M7**: User-supplied art import, character rigging, lighting, UI, build pipeline for Windows.

## Definition of done
Current family release: Windows 0.5.0 packages music/effects, branded menus, Input System only, photo-guided cats and the local park/school walk. Launch PLAY.cmd or builds/Current/Cheweenie.exe. Scene presentation ownership supports empty and default Unity scenes. Pause Settings includes Return Marty home without losing progress. See docs/VALIDATION.md for actual release results and docs/ASSUMPTIONS.md for editable defaults, inferred details and placeholders. Physical controllers and subjective speaker mix remain human checks. M7 still needs final Marty/house art and rigging; existing visuals are playable prototype art.

Project opens without missing packages, playable initial quest, no console errors, control support verified with controller, screenshots, documentation, and clear deployment instructions.

## Codex setup
In Codex, select GitHub repo `Ellis1717/codename-cheweenie`, create a cloud task using the starter prompt, and branch from `main`. If a Unity editor is absent in cloud, perform static validations and mark runtime checks as pending. No Codex session has been launched by this document alone.

Neighbourhood extension (2026-10-29): a fictional walking block, open east gate, houses/park/garden, live map and independent saved sniff patrol are implemented on the neighbourhood branch. Verification: 46 EditMode tests, 4 PlayMode tests, 145 standalone checks and 26 built-player acceptance checks pass. See docs/NEIGHBOURHOOD.md and docs/VALIDATION.md. Real neighbourhood likeness still requires family-provided landmarks; no real addresses were inferred.

Private local-map support (2026-10-29): ignored home-relative map data can replace the default fictional block. Includes public street geometry, park/school sites, expanded map controls, a four-stop saved patrol and a local OSM importer. Houses/terrain and short walking connectors remain schematic. No home address or personalized map is published. Validation: 53 EditMode, 4 PlayMode, 145 standalone, 30 full-walk checks and 11 final-build menu checks pass. See docs/LOCAL_MAPS.md and docs/VALIDATION.md.

Input migration (2026-10-29): Active Input Handling is Input System Package (New) only. No runtime Input Manager reads remain. IMGUI menus use explicit Input System mouse hit-testing with GUI scale and single-press consumption. Valid saved binding IDs are retained; unsupported character bindings recover defaults. Verification: 53 EditMode, 4 PlayMode, 145 standalone and 14 rebuilt-player input/menu checks pass. The user's main Unity editor still needs a restart to change its live backend.

Photo-guided cats (2026-10-29): original cat photos recovered from the earlier chat. Both cover and CatVisual now use Lakitu's asymmetric white/black face and Rio's mostly black coat/white bib; original root physics, quests and saves are retained. Raw photos remain ignored in PrivateReferences outside Unity. Verification: 56 EditMode, 5 PlayMode and 17 rebuilt-player startup/visual/input checks pass. Rendered cat portraits and updated title/settings screenshots are in docs/screenshots; see BRANDING.md for reference details and honest stylization limits.
