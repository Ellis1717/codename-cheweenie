# Marty’s fur at gameplay distance — 0.11.5

The short coat previously put all three directional texture layers at nearly the same fine scale. Pixel filtering removed them together as Marty became smaller on screen, leaving a smooth-looking body. The new `MartyPhotoReference/GameplayFibres` materials combine individual hairs with two broader groups of overlapping short hairs. Each scale fades smoothly with its projected pixel footprint. The broader layers retain directional texture after individual hairs become too small to resolve; at very long distance the coat still filters to its average colour.

The original UV colour decoder, 4K maps, cloth microfibre shading and 32,000 geometric fibres remain. This changes material detail without altering Marty’s photo-guided proportions, reference skeleton, controls or collision. Earlier materials remain in `MartySoftCoat/DirectionalFibres`; the separate smooth model remains available through **Detailed coat**. Cats, Unity and the independent preview are unaffected. This is procedural fur shading and ribbon geometry, not a strand Groom.

## Rebuild and review

From `Unreal/UE5MovementLab`:

```powershell
./BUILD.ps1
./IMPORT_GAMEPLAY_FUR.ps1
./TEST_GAIT.ps1 -Render -OutputRoot D:/CheweenieValidation/GameplayFur/EditorGait
./TEST.ps1
./PACKAGE.ps1 -OutputRoot D:/CheweenieValidation/GraphicsLabBuilds/UnrealBorkOrTreat-0.11.5
```

The importer creates four dedicated materials and changes only the current detailed mesh’s material assignments. Reimporting the likeness model preserves these assignments when the new materials are available. Shader source is retained in `Scripts/shaders/marty_gameplay_fur.ush`. The shader’s content hash selects a separate material revision folder. Editing the shader and rerunning the importer therefore retains earlier graphs and never deletes expressions from a loaded material.

Rendered gait acceptance captures the close portrait, matching full-body overview, normal gameplay camera (`MartyFurGameplay.png`) and a farther 65-degree camera (`MartyFurFar.png`) in the actual game map. It exercises synthetic keyboard and controller movement and checks all four cloth coat materials. Visual review is separate from these assertions; neither constitutes a physical controller or broad performance benchmark.

## Actual results and play

Reopen **Play Cheweenie Bork or Treat** and enable **Detailed coat** in Controls. The local shortcut selects the hash-gated 0.11.5 package. The stored 0.11.4 checkpoint and local map resources were copied only when absent; originals and earlier packages remain available. This material pass is separate from the other chat’s denser-geometry hair preview.

Native Editor/Game compilation passed. Full cooking/archive passed before the last material-only frequency adjustment; its final incremental recook/archive also passed. The final executable passed **420 assertions across 22 packaged reports**, plus a tan-coat pixel probe. The Editor passed all 16 movement checks on the headless rerun. Two 30-assertion rendered Editor gait runs passed before the final distant-band adjustment; the final shader is covered by the packaged 30-assertion rendered gait run. The release gate verified 43 payload hashes, fresh captures and zero final runtime Error/Fatal/material fallback/VSM overflow matches. Windows PowerShell launcher validation passed with actual game launch suppressed.

Six final packaged images were reviewed: [overview](screenshots/ue5-marty-gameplay-fur-overview.png), [portrait](screenshots/ue5-marty-gameplay-fur-portrait.png), [gameplay camera](screenshots/ue5-marty-gameplay-fur-camera.png), [far camera](screenshots/ue5-marty-gameplay-fur-far.png), [meal](screenshots/ue5-marty-gameplay-fur-meal.png) and [costume](screenshots/ue5-marty-gameplay-fur-costume.png). This improves the impression of a short coat; individual hairs still filter to their average at sufficiently small screen sizes.

The [evidence record](evidence/ue5-marty-gameplay-fur-build.json) retains initial findings and corrections. The first broad pattern looked grooved, the first far camera met a pillar, and expression deletion during a reimport asserted; staggered softer marks, an unobstructed camera and immutable revision folders address those findings. One earlier rendered movement run missed the strict recenter check (15/16); the unchanged controller code passed 16/16 headlessly. The first unlocked LAN run preceded the tribute unlock fixture; its final rerun passed after that fixture. When reproducing the full package gate, run `TEST_RATCHET.ps1 -Render` before `TEST_COOP.ps1 -Ratchet`, then `FINALIZE_FESTIVAL.ps1`.

Physical controllers, separate PCs, owner coat approval and broad frame-time comparisons remain unverified. Unity was not retested because its source, assets and settings were not changed.
