# Retained cozy scenery

This original art set matches the stylized Explorer/pet models with warm matte materials, softened furniture edges, simple foliage and readable walking surfaces. It is not a surveyed reconstruction or a photorealistic recreation of the supplied concept illustration.

`Assets/Resources/SceneryPalette.asset` connects twelve textured materials and thirteen reusable prop prefabs: sofa, table, kitchen island, cushion, picket fence, tree, shrub, flower planter, framed window, bench, park play frame, rug and cottage roof. Their meshes, PNG patterns, materials, prefabs, metadata and authoring scripts are retained in Git. The repeating surface patterns are generated from original code; no paid assets or downloaded artwork are required.

`WorldArt` applies this visual layer to the tested headquarters/backyard/neighbourhood: wood/plaster/stone/grass surfaces, rounded furniture, windows/doors, gabled neighbour roofs, fences, plants, park props and school details. Original solid colliders, paths, ramp channels, marker positions and mission layout remain. Added props are cosmetic and do not add gameplay hitboxes; the park frame is scenery, not a new climbing activity. The headquarters stays open above for camera visibility. House/people geometry remains schematic.

Props share retained meshes/materials, use instancing where supported, and have distance-based visibility. Warm daylight, soft shadows, ground/sky ambient colours and distance haze are applied while the prototype owns the scene. ScenePresentation restores the original fog, ambient settings and shadow distance when that world is destroyed. User scene objects are excluded from material replacement.

Edit materials/PNGs/prefabs or the SceneryPalette reference fields in Unity. **Cheweenie > Art > Create or rebuild cozy scenery assets** regenerates prop geometry while retaining existing material/texture assets. Back up modified geometry before explicitly rebuilding. Normal builds preserve existing art. Code: SceneryAssetBuilder, RetainedArtWriter, WorldArt, SceneryPalette and ScenePresentation. Keep assets and metadata together.

Private street geometry remains in the ignored map resource. The public art assets contain no home coordinates, original reference photos or household/person backgrounds. Shareable art captures hide private street geometry and distant map-dependent props and use a neutral temporary ground backdrop; normal gameplay uses the actual selected map.
