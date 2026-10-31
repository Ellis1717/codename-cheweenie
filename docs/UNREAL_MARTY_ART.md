# Marty: cover-guided skin and canine movement

Version 0.7 adds a separate short-coat variant with 4K maps, fine skinned fur, whiskers, checker collar and photo-informed colour/proportion changes. It also extends poses for lying, rolling, paw shaking and body sway. [0.7 source and controls](UNREAL_MARTY_07.md). The prior 0.5 sources described below remain retained.

The start-page cover at Assets/Resources/Branding/Cover.png is the owner's selected visual target. The new Marty variant is original geometry authored with Blender: continuous torso/neck/head, tapered muzzle, amber eyes with wet highlights, a dark nose, floppy ears, short golden coat grain and rounded paws. The original Explorer OBJ/GLB, Unity characters and static Unreal comparison pets are preserved. This is a step toward the cover's realism; the illustration is not an exact render of this mesh. Lakitu and Rio retain their earlier static meshes in this milestone.

## Inspect in the actual game

Launch Play Cheweenie Unreal or UNREAL_PLAY.cmd. Start/continue the patrol. Press V or R3 three times to reach the fourth, close inspection camera. Orbit with mouse/right stick to see Marty from the side or front. WASD/left stick walks; Shift/A-Cross hurries; Escape/Menu pauses. The existing three camera distances remain available by cycling again. The camera is deliberately close for inspection and can encounter furniture; cycle out for normal exploration.

Marty has a true skin-weighted skeletal mesh with 24 named joints. Native procedural IK drives it; there are no purchased animation clips, motion capture, Control Rig or strand groom dependencies. Foreleg elbows bend backward, hind stifles bend forward, and wrists/hocks and paws articulate separately. Four-beat walk phases blend toward diagonal trot timing as speed increases. Actual displacement advances the step cycle. Stance targets stay in world space, ground traces place contacts, and swing curves lift paws. Long displacements reset contacts instead of stretching legs across the room. Pause freezes the pose; idle adds breathing and a small tail wag. Ears have separate base/tip weights and phase-lagged motion, so their ends flex rather than rotating an entire rigid ear.

The short-coat normal/grain textures and roughness variation improve surface response without a costly fur groom. The ear motion is procedural rather than a cloth/physics simulation. The gait is tuned for this small, level demo room, not stairs, jumping or all animal gaits. Contact limits can still need art tuning during sharp turns. Real controller feel and subjective likeness should be checked by the owner.

## Source and reproducibility

- Unreal/UE5MovementLab/SourceArt/Marty retains the editable Blender scene, skinned FBX, original generated coat textures and a provenance manifest.
- Scripts/build_marty_rig.py regenerates only this new model using Blender 5. Run Blender in background with --python and the absolute script path. It uses bundled numpy and no external geometry. Explicit regeneration replaces this generated variant; save custom art in a separate file first.
- IMPORT_MARTY.ps1 imports into /Game/Adventure/Marty only. Explicit import replaces the generated mesh and textures and preserves existing material edits; it does not touch the independent preview or Unity. Edit the saved materials in Unreal to tune coat, nose and eye appearance.
- Source/CheweenieUE5Lab/MartyGait.h/.cpp owns the solver. WalkStride, TrotStride and PawLift are editable component properties; higher values are centimetres, not metres.
- The actor retains the static fallback. Launch with -CheweenieLegacyMarty for a same-map visual comparison; the archived 0.4 package remains separately available.

Rebuild order: Blender source generation → IMPORT_MARTY.ps1 → BUILD.ps1 → TEST_GAIT.ps1 -Render → TEST_ADVENTURE.ps1 -Render → PACKAGE.ps1 → run the same tests with -GameExe pointing at the package. Close this project's editor/game before compilation or packaging to avoid a locked native module. Builds/cache/logs are ignored. Original source art and generated owned Unreal assets are versioned together.

The native gait test uses an isolated CheweenieGaitAcceptance slot, validates the real imported bones and input-driven motion, and deletes only its own slot. Acceptance evidence and actual Unreal screenshots are recorded alongside the milestone report. Passing synthetic tests does not prove physical hardware compatibility or photorealism.

The final 0.5 Windows package passed all 14 gait assertions under DX12. The retained source now has 41,160 vertices, closed limb/ear/tail ends and outward-facing normals. Actual captures are docs/screenshots/ue5-05-marty.png, ue5-05-walk.png and ue5-05-trot.png. Ear-tip angle change, elbow/stifle bending, measured-travel stepping, controller hurry, pause and stable return-home idle passed. The initial missing skeletal material usage/save, open surfaces and camera framing were corrected before final packaging. This remains stylized art with procedural movement, not the cover's full photographic detail.
