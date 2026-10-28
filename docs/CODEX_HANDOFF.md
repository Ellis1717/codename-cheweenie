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
Source status on 2026-10-28: M1–M6 have implementations on development branches, including branded title/settings and M6's schematic backyard adventure. Acceptance remains pending: the installed Unity editor has no active license. The standalone suite passes 112 checks; C# compilation is not Unity gameplay verification. See docs/VALIDATION.md and docs/BACKYARD.md. M7 still requires user-supplied art and licensed Unity import/build checks.

Project opens without missing packages, playable initial quest, no console errors, control support verified with controller, screenshots, documentation, and clear deployment instructions.

## Codex setup
In Codex, select GitHub repo `Ellis1717/codename-cheweenie`, create a cloud task using the starter prompt, and branch from `main`. If a Unity editor is absent in cloud, perform static validations and mark runtime checks as pending. No Codex session has been launched by this document alone.
