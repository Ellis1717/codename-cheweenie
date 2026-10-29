# Retained Marty character resources

The default **MartyVisual** prefab uses the owner's supplied **Marty_Explorer.obj** mesh. Both original OBJ and GLB from the supplied character archive are retained byte-for-byte under `Source/`. The static source has **7,666 vertices, 15,136 triangles and ten colours**. The importer preserves all geometry and colours, fits it uniformly to the gameplay root, and attaches disconnected surfaces to animation joints.

## Included and version controlled

- `Assets/Resources/MartyVisual.prefab`: the actual runtime Explorer character.
- `Source/Marty_Explorer.obj` and `.glb`: original interchangeable source files. The GLB has no skin, animations or embedded images; the OBJ supplies geometry and vertex colours.
- `Explorer/MartyExplorerMeshes.asset` and ten `Source_*.mat` files: Unity meshes/materials derived from the original OBJ. The rig records the source SHA-256 and vertex/triangle counts.
- `Assets/Editor/MartyExplorerImporter.cs`: the reproducible importer, **Cheweenie > Art > Import supplied Marty Explorer model**. No paid or glTF plugin is needed. It supports the supplied triangular, per-surface-colour OBJ format, rather than being a general OBJ/glTF importer.
- `Assets/Scripts/MartyModelRig.cs`: basic articulated legs/feet, head, ears, tail and blinking. The original download is static; this adds motion without replacing topology. It is not a skinned FBX rig.
- `MartyPhotoGuided.prefab`, `MartyMeshes.asset` and eight named materials: the separately authored photo-guided alternative, retained for comparison/editing.
- `Assets/Editor/MartyModelBuilder.cs`: **Cheweenie > Art > Create or rebuild photo-guided Marty**, which updates the alternative without changing the default Explorer prefab.

The supplied concept sheet is an illustration with more detail than the static mesh. Importing the OBJ does not reproduce its fur, animation poses or detailed environment automatically. Original photos and the complete concept sheet remain in ignored `PrivateReferences/InitialBuildout`; their household/person backgrounds and private house photo are not published or bundled. The game meshes contain no personal tag numbers.

## Editing and preservation

Edit `Explorer/Source_*.mat` to tune the default model. The normal Windows build preserves an existing default prefab and material edits. Explicitly importing Explorer regenerates its joint hierarchy/meshes while retaining existing source-material values and mesh GUIDs. Commit source files, prefab, mesh/material assets, `.meta` files and scripts together.

The default uses +Z forward/+Y up, uniform scale 0.78 and a root offset so feet fit the original capsule. Joint assignments use the supplied model's disconnected surfaces and anatomical positions. There are no visual colliders or Rigidbody; gameplay movement, interactions and save identity remain on MartyController.

To use the photo-guided alternative, back up the default, then copy the alternative prefab to `Assets/Resources/MartyVisual.prefab` through Unity. To use a future imported rig, replace that resource with its prefab, materials and optional Animator. The Explorer importer refuses to overwrite a custom default without MartyModelRig. A missing default is rebuilt from retained Explorer source during a Windows build; the runtime primitive fallback is only an emergency path.