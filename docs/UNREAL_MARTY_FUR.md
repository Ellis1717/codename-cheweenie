# Marty’s combed short coat — 0.11.3

The owner’s photographs show a short tan coat, darker ears and a lighter underside. This pass improves that coat on the existing stylized anatomy. It retains the connected ear/tail skin and the same 24-joint reference rig.

The new `MartySoftCoat` source and runtime folders retain every older model. The coat has 32,000 skin-weighted crossed micro-ribbons, up from 16,000. They are 1–2.8 mm long on the body, shorter on the face/ears, and lie closer to the surface. Smooth surface normals and restrained per-strand colour variation reduce the old upright dark-speck appearance. The four new coat materials use Unreal’s cloth BRDF for microfibre scattering; this is a shading approximation, without cloth simulation, a strand Groom or a paid plugin. Slightly lighter strands and directional microtexture keep visible coat detail.

The original 4K grain and normal maps are reused unchanged. A softer normal response, rougher highlights and fibre fuzz replace the earlier hard hair shading. The early passes looked too smooth in actual gameplay. The final `DirectionalFibres/` materials add original procedural hair ridges and staggered strand tone in the moving skin’s UVs, blended with the retained normal map. Pixel-footprint filtering fades this detail with distance. Earlier materials remain retained in the parent and `Fibres/` folders. The fibre-weight input is connected through MakeMaterialAttributes because UE5.8 hides CustomData0 from Python’s MaterialProperty enum. Import completion requires the final material report and clean shader logs, rather than the intermediate mesh report.

## Play and adjust

Reopen **Play Cheweenie Bork or Treat** for the validated 0.11.3 package. Keep **Detailed coat** enabled in the Controls feature switches. Disabling it still selects the retained smooth connected model; previous preferences are respected. Costumes and the existing keyboard/Xbox/PlayStation mappings are retained.

The denser model has 339,056 vertices, compared with 211,056 previously. Its source audit caps this at 340,000 and checks normalized weights, custom normals, unchanged bind matrices, retained map hashes and one closed primary skin. Fine-fur sections do not cast their own shadows; the body still does. This adds vertex work and does not claim a measured performance improvement. The smooth model remains the lighter option.

## Rebuild

From `Unreal/UE5MovementLab`:

```powershell
& 'C:/Program Files/Blender Foundation/Blender 5.0/blender.exe' --background --factory-startup --python Scripts/build_marty_softcoat.py
& 'C:/Program Files/Blender Foundation/Blender 5.0/blender.exe' --background --factory-startup --python Scripts/audit_marty_softcoat.py
./IMPORT_SOFTCOAT.ps1
./BUILD.ps1
./PACKAGE.ps1 -OutputRoot D:/CheweenieValidation/GraphicsLabBuilds/UnrealBorkOrTreat-0.11.3
```

Use the packaged suites and `FINALIZE_FESTIVAL.ps1` before switching the launcher. Actual results and rendered comparison captures are in [the evidence record](evidence/ue5-marty-fur-build.json). Input checks are synthetic; physical controllers, separate PCs, subjective likeness/listening and broad frame-time benchmarks remain unverified. The cats’ models and the scenery are outside this fur pass.

Final 0.11.3 verification: native Editor/Game builds and full cook/archive passed. The final executable passed 418 packaged assertions and the Editor passed 16 movement assertions. The final shader also passed all 28 rendered Editor gait checks. The release gate recorded 43 payload hashes, fresh root/costume/seasonal captures and zero runtime Error/Fatal/material fallback/VSM overflow matches. Four final coat/action/costume images were visually reviewed. The Windows PowerShell launcher gate passed without opening another game, and the shortcut now selects 0.11.3 while retaining the local resources and checkpoint.
