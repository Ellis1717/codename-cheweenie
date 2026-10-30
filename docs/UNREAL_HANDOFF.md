# Codename .... Cheweenie — Unreal development handoff

The owner has chosen to set Unity aside and continue with Unreal Engine. Unreal 5.8.4 is the active development target. Retain the working Unity 0.9.0 game, source, art, photos policy and launchers as a fallback. This decision does not authorize deleting existing resources. The previous Unreal 4.27 experiment also remains available.

## Current state

- `Unreal/UE5VisualLab`: content-only 59-actor room preview with the three retained static pets, materials and Lumen. Native asset audit and an offscreen rendered capture passed. Launch with `LAB_UE5.cmd`.
- `Unreal/UE5MovementLab`: separate native movement prototype source, stacked on the preview in PR #19. Character movement, Enhanced Input, three cameras, recovery and a controls HUD are prepared. Native compilation and gameplay have not passed. Its build materializes an independent Content copy from the retained preview.
- Installed MSVC v14.42 is banned by UnrealBuildTool 5.8.4. A side-by-side v14.38 install succeeded, but a real compile failed inside UE5.8 engine headers. Current v14.44+ tools are required; the 17.12 installed product graph cannot add v14.44 without an update. Automatic approval review blocked the separate Build Tools download/install attempt. Follow the movement README for setup and acceptance steps. Do not override the compiler ban or patch engine headers to make an old compiler pass.
- Character models remain stylized static meshes. Improved lighting is not a finished realistic rig. Preserve Marty resources and Lakitu/Rio's photo-guided markings; do not present concept art as gameplay.

## Ordered milestones

1. **U1 — playable traversal:** resolve the compiler prerequisite, compile and fix the native module, launch the actual room, verify floor/furniture/boundary collisions, analog movement, stopping/turning, all cameras and return-home. Run keyboard and virtual input checks where available; record physical controller checks separately. Retain an actual gameplay screenshot and test results before marking the prototype playable.
2. **U2 — one moving Marty:** prepare a reproducible articulated or skinned character pipeline from retained geometry and references, preserve original assets, add grounded walk/trot/idle/turn/Cozy motion, and inspect paws, proportions and camera presentation in play. State clearly whether movement is procedural articulation or a skin-weighted animated rig. Improve coat, eye and fur detail based on actual renders.
3. **U3 — matching companions:** apply the accepted character approach to Lakitu and Rio, preserving Lakitu's asymmetric black/white face and Rio's black face/white chin/bib. Add follow, idle and interaction behavior with smooth starts/stops and independent tuning.
4. **U4 — family demo loop:** add branded title/menu, controller-friendly settings/help, pause/resume, camera/volume options, original music/effects, simple progress and the Fox Ellis/Leo Ellis cameos. Implement Cheween in Between and Cheeween Cozy as playable interactions. Port behavior deliberately; Unity saves and quests are not automatically compatible.
5. **U5 — finished small environment:** improve one cozy room, then expand headquarters/backyard and neighbourhood routes. Maintain traversable collision and readable lighting. Keep private address/reference data out of public commits. Treat schematic layouts as assumptions, not measured reconstructions.
6. **U6 — distributable Windows demo:** package a build that does not require the editor, verify startup/menu/gameplay/controller recovery/audio and record measured performance on this machine. Provide a simple launcher and keep the Unity fallback available.

## Working rules

Keep the exact title **Codename .... Cheweenie**, canon, aliases and user-owned art. Use self-contained, appropriately licensed resources and no paid dependencies. Never commit engine caches, build products, private references or secrets. Keep assumptions editable in docs/ASSUMPTIONS.md and report failed checks alongside passes. Use focused PRs; do not merge an uncompiled gameplay prototype as a verified release.
