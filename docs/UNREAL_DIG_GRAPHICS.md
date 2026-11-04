# Digging graphics and depth

**Superseded terrain approach:** the raised-bowl prototype described below is replaced by [real downward pits](UNREAL_DIG_DOWNWARD.md), following the owner's correction that digging should go down. The old captures and results remain as historical evidence.

Dig patches in the native Unreal solo adventure now have three visually distinct stages: **shallow (1/3), medium (2/3), and deep (3/3)**. Each completed dig widens the opening, increases the soil relief and exposes more concentric earth layers around a dark center. Loose clumps form a rim and spoil pile. Burying a toy visibly fills the center with a dirt mound; recovering it removes that cover and reopens the deep hole.

During **F / RT-R2** digging, small soil clods fly backward from both front paws, driven by the same phase as Marty's existing paw animation. Marty faces the dig patch and stops moving for the action. A nearby depth card shows three filled segments and the current dig's progress. A floating label identifies the current stage and buried state. **E / Xbox X / PlayStation Square** still buries a carried toy in a fully dug patch.

The depth card follows the Prompts overlay switch, and floating labels follow both the World labels feature and overlay settings. The overlay master applies to both. The Dig feature suppresses new digging effects and the depth card while preserving existing hole geometry and saved progress. Pausing freezes airborne soil and action progress. Patch proximity also checks height so digging cannot reach through another floor.

This is cosmetic excavation over the retained terrain. The recessed appearance is formed by raised soil layers around a lower, dark center; it does not carve terrain or change walking collision. Depth numbers are gameplay stages, not surveyed centimetres. Existing energy costs, timing/upgrades, dig patches and toy saves are retained.

`UDigVisuals` uses bounded instanced engine cubes/spheres and the existing project material. It rebuilds appearance from `HoleDepths` and `ItemHoles`, adds no save fields, and needs no paid assets, plugins or imported resources. Unity, the content-only preview and LAN room are unchanged.

Native Editor compilation passed. The rendered real-map life suite passed **40 assertions**, and its second-process persistence phase passed **7**. These include every visual depth stage, airborne soil/progress, bury/recover graphics, feature switches and saved mound/depth reconstruction. Movement (16), home/restart (34 + 5), navigation/restart (60 + 4), upstairs (18) and overlays/restart (79 + 3) passed **219 regression assertions**: **266 selected assertions total**. The selected current logs contained zero Error/Fatal/material fallback/VSM overflow matches.

Five actual captures were visually reviewed: [digging action](screenshots/ue5-dig-action.png), [shallow](screenshots/ue5-dig-shallow.png), [medium](screenshots/ue5-dig-medium.png), [deep](screenshots/ue5-dig-deep.png), and [buried toy](screenshots/ue5-dig-buried.png). See [life checks](evidence/ue5-dig-graphics-life.json), [restart checks](evidence/ue5-dig-graphics-resume.json), and [build, regressions and scope](evidence/ue5-dig-graphics-build.json).

Build with `Unreal/UE5MovementLab/BUILD.ps1`. Run `Unreal/UE5MovementLab/TEST_LIFE.ps1 -Render -OutputRoot ./Unreal/UE5MovementLab/Saved/DigGraphics` to reproduce the action/depth/bury/recovery and persistence checks. Input was synthetic keyboard/gamepad with teleported position/energy fixtures. Physical controllers, comparative performance and a new desktop package remain unverified. Existing unrelated checkout edits were retained; no PR was opened from this mixed checkout.
