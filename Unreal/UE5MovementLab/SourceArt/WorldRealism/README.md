# Original environment source — Codename .... Cheweenie

All geometry and raster maps in this directory are original procedural authoring output. No downloaded models, paid packs, photographs, private household imagery or copied artwork are used. The abstract wall reliefs are generated geometry. They are not scans or a measured reconstruction of the owner's house.

WorldRealism.blend retains the full editable source scene. Twelve FBX meshes separate architecture, floor, sofa, soft furnishings, room details, kitchen, plants, blanket fort, tree, bench, grass clump and gable infill. Seventeen surface families have base-colour/normal/roughness PNGs (51 maps): oak is 2048 square, the others 1024. Brass, dark metal, steel and opaque window glass use material constants. The runtime exterior also uses eight world-space material variants.

Regenerate with BUILD_WORLD_ART.ps1 (Blender 5). The main build_world_art.py calls build_world_fort.py, build_world_outdoor.py, refine_world_surfaces.py and build_world_kitchen.py in order. The manifest records mesh topology, material roles and generator hashes. IMPORT_WORLD.ps1 imports only /Game/Adventure/Scenery into the native movement project. Base colour is sRGB; normals and roughness are linear. Texture source hashes trigger reimports when procedural sources change. Weighted upholstery normals are imported rather than discarded.

Blender centimetre coordinates are reflected in Y by the FBX scene conversion; the native visual actors explicitly mirror that axis to match the retained gameplay coordinates. The original furniture/floor/boundary colliders are preserved separately. The new visual overlays have actor and component collision disabled. Outdoor roof infill/window trim is cosmetic; corrected roof slopes have no collision. Grass uses one 640-instance batch with a short cull distance; window trim is another batch, and decorative actors do not tick.

The tree has connected woody twigs and thin serrated leaves, not scattered floating spheres or a scanned/wind-simulated canopy. The room and materials improve realism; the pets, house footprints, school and neighbourhood still contain stylized/schematic elements. Further hero assets, landscaping, measured performance and physical controller review remain future work.

The material roles follow [Epic's physically based material guidance](https://dev.epicgames.com/documentation/unreal-engine/physically-based-materials-in-unreal-engine). Original generators, meshes and maps remain versioned separately from all retained Unity/comparison/pet assets.
