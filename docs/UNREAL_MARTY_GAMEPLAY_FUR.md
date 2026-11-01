# Marty’s fur at gameplay distance

The short coat previously put all three directional texture layers at nearly the same fine scale. Pixel filtering removed them together as Marty became smaller on screen, leaving a smooth-looking body. The new `MartyPhotoReference/GameplayFibres` materials combine individual hairs with two broader groups of overlapping short hairs. Each scale fades smoothly with its projected pixel footprint. The broader layers retain directional texture after individual hairs become too small to resolve; at very long distance the coat still filters to its average colour.

The original UV colour decoder, 4K maps, cloth microfibre shading and 32,000 geometric fibres remain. This changes material detail without altering Marty’s photo-guided proportions, reference skeleton, controls or collision. Earlier materials remain in `MartySoftCoat/DirectionalFibres`; the separate smooth model remains available through **Detailed coat**. Cats, Unity and the independent preview are unaffected. This is procedural fur shading and ribbon geometry, not a strand Groom.

## Rebuild and review

From `Unreal/UE5MovementLab`:

```powershell
./IMPORT_GAMEPLAY_FUR.ps1
./BUILD.ps1
./TEST_GAIT.ps1 -Render -OutputRoot D:/CheweenieValidation/GameplayFur/EditorGait
./TEST.ps1 -Render
```

The importer creates four dedicated materials and changes only the current detailed mesh’s material assignments. Reimporting the likeness model preserves these assignments when the new materials are available. Shader source is retained in `Scripts/shaders/marty_gameplay_fur.ush`. The shader’s content hash controls the material revision, so editing the shader and rerunning the importer rebuilds the graph without accumulating old expressions.

Rendered gait acceptance captures the close portrait, matching full-body overview, normal gameplay camera (`MartyFurGameplay.png`) and a farther 65-degree camera (`MartyFurFar.png`) in the actual game map. It exercises synthetic keyboard and controller movement and checks all four cloth coat materials. Visual review is separate from these assertions; neither constitutes a physical controller or broad performance benchmark.

Validation and packaged availability will be recorded after the actual runs.
