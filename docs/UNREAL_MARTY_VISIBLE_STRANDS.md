# Marty’s raised short fur

This pass adds visible geometric hairs to **Codename .... Cheweenie**, keeping the photo-guided body, face, ears and tail. The separate `MartyVisibleStrands` model doubles the coat to 64,000 tapered ribbons. Body strands are authored at 2–4.8 mm, with shorter face/ear hairs, wider roots, modest outward lift and restrained colour variation. This deliberately increases visual coverage while retaining a short coat.

The model has 335,678 source vertices, up from 207,678. More geometry adds rendering and memory work; a performance improvement is not claimed. Existing input mappings, animation, outfits and saves are preserved.

## Choose fur detail

In the solo game, open **Pause → Controls → Camera, graphics & sound → Fur detail**. Left/right arrows or D-pad change the setting. Enter / Xbox A / PlayStation Cross, or clicking the row, cycles forward. Mouse minus/plus buttons also adjust it. The change takes effect immediately and is saved.

| Level | Appearance |
| --- | --- |
| Low | Same photo-guided shape, without geometric hairs (79,678 vertices) |
| Medium | Photo-guided model with 32,000 short hairs |
| High | Photo-guided model with 64,000 wider, raised short hairs |

New saves default to High. Older saves that disabled the detailed coat remain Low. The existing coat on/off flag still works and remembers the last enabled level; it does not reset patrol progress or other preferences. The new saved preference is bounded during loading. These are solo graphics controls; the separate LAN room currently uses its existing default pet presentation.

The Blender audits verify that Low and High retain all 24 reference joints from `MartyPhotoReference`, every primary-skin vertex has the same position, ear/tail skin remains closed, weights are normalized and the 4K texture hashes match. High also verifies outward-facing fur faces. Low preserves the original surface normals while removing only hair geometry. Medium and High share the distance-filtered undercoat materials, so broader overlapping fur texture remains readable as individual hairs become subpixel. The source manifests record strand dimensions and variation. This uses geometric ribbons rather than a strand Groom or cloth simulation. Earlier models, materials, Unity, the independent preview and packages remain intact.

## Rebuild

From `Unreal/UE5MovementLab`:

```powershell
& 'C:/Program Files/Blender Foundation/Blender 5.0/blender.exe' --background --factory-startup --python Scripts/build_marty_visible_strands.py
& 'C:/Program Files/Blender Foundation/Blender 5.0/blender.exe' --background --factory-startup --python Scripts/audit_marty_visible_strands.py
./IMPORT_VISIBLE_STRANDS.ps1
& 'C:/Program Files/Blender Foundation/Blender 5.0/blender.exe' --background --factory-startup --python Scripts/build_marty_photo_smooth.py
& 'C:/Program Files/Blender Foundation/Blender 5.0/blender.exe' --background --factory-startup --python Scripts/audit_marty_photo_smooth.py
./IMPORT_PHOTO_SMOOTH.ps1
./BUILD.ps1
```

Run `IMPORT_GAMEPLAY_FUR.ps1` before the raised-hair import when rebuilding materials. The raised-hair importer links its four coat slots to the current immutable Medium material revision; later material imports also update High when that asset exists. Low retains the earlier fine surface shader. Original materials and revisions are preserved.

## Validated preview

Open **Play Cheweenie Hair Preview** for 0.11.6. Its package is independent of the retained Bork or Treat 0.11.5 launcher. The player checkpoint and two private resource files were copied only when absent and checked by SHA-256; originals remain. This preview opens the existing optional Halloween menu. Start the game, then use Pause → Controls to adjust fur.

Final native Editor/Game builds and full Windows cook/archive passed. The final executable passed **443 assertions**, including 23 quality/save checks across three processes, 30 gait/material checks and the retained costume, world, navigation, companion, patrol, life, festival, Ratchet and locked/unlocked two-process LAN regressions. The Editor movement test passed 16; another Editor quality preflight passed 23 and is excluded from the 459-assertion release total. All 43 payload hashes, fresh capture dimensions and the tan-coat pixel probe passed. Runtime Error/Fatal/material fallback/VSM overflow matches were zero.

Eight actual packaged images were reviewed: [controls](screenshots/ue5-marty-hair-detail-menu.png), [head](screenshots/ue5-marty-hair-head.png), [body](screenshots/ue5-marty-hair-body.png), [meal](screenshots/ue5-marty-hair-meal.png), [overview](screenshots/ue5-marty-hair-overview.png), [gameplay](screenshots/ue5-marty-hair-gameplay.png), [farther view](screenshots/ue5-marty-hair-far.png) and [costume](screenshots/ue5-marty-hair-costume.png). These use the real map with camera/pose fixtures. Individual hairs blend into undercoat texture when they become smaller than pixels. The overview reaches the lower image edge at a front paw; the gameplay and costume views show the complete character.

The first controls fixture held two aliases of one Enhanced Input action without a release frame; separating the controller/keyboard handoffs fixed that test sequence. The preservation fixture was then established after gameplay paused. All final switches, saved preferences and preservation checks passed. [Actual reports, source audits and hashes](evidence/ue5-marty-hair-detail-build.json) retain the validation scope.

Physical controllers, separate-machine networking, broad frame-time/memory benchmarks, subjective listening and owner likeness approval remain unverified. This is geometric short fur and procedural fibre shading; individual hairs do not have separate physics, and the underlying anatomy remains stylized.
