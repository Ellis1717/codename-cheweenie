# Original articulated companion assets

`Scripts/build_companion_rigs.py` generates five original smooth, skin-weighted meshes in Blender 5: Bauer, Hank, a neutral potcake shared by Georgia/Pieper and the editable slots, Lakitu and Rio. All use centimetres and 27 named joints. `Scripts/import_companion_rigs.py` imports them into `/Game/Adventure/CompanionRigs` and creates skeletal-compatible materials with the project's original coat normal.

The bodies are continuous voxel-unioned surfaces; lofted limbs interpolate weights through elbow/stifle, wrist/hock and paws. Pelvis, spine, chest, neck, head, ears and three tail joints deform the skin. Cats have upright ears, smaller muzzles, slit pupils and tapered whiskers. Their established black/white facial and chest cues are retained. Bauer is golden; Hank is larger and black/white. Potcake coats are neutral placeholders, not claims about the owner's dogs.

These are original procedural stylizations, not scanned likenesses, final photoreal fur or motion capture. Retained Marty, Unity resources, static cat meshes and the independent content preview are preserved. The FBX files plus generator and manifest are the editable source; no paid tools/add-ons or downloaded geometry are required beyond free Blender and Unreal.
