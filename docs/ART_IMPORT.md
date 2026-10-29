# Marty art pipeline

The default Marty character now uses the original `Marty_Explorer.obj` supplied in the owner's archive, imported into `Assets/Resources/MartyVisual.prefab`. Both OBJ and GLB, derived Unity meshes/materials, animated joint hierarchy and reproducible importer are retained in Git under `Assets/Art/Marty` and the editor/runtime scripts. A separately authored photo-guided model is retained as an alternative. See [resource provenance and editing](../Assets/Art/Marty/README.md). The old primitive canine is only the missing-prefab fallback.

Import an owned or licensed FBX into `Assets/Art/Marty/`. Use metres, +Z forward and +Y up. Put a Generic rig and Animator on a prefab with its paws about 0.75 metres below the gameplay root. Adjust the visual child transform to fit the gameplay capsule. Disable root motion. Optional Animator parameters: `Speed` (float), `Cozy` (bool).

Save the prefab as `Assets/Resources/MartyVisual.prefab`. Bootstrap loads it automatically. Visual colliders are disabled; the independent Rigidbody/capsule owns movement. Keep materials and textures inside the project. No art is downloaded by the game.

Create a **Cheweenie / Canine Tuning** asset at `Assets/Resources/MartyTuning.asset` to tune acceleration, speeds, turn rate, jump speed and interaction distance. Test keyboard, controllers and animation after importing. The retained model remains a stylized likeness, with unseen anatomy inferred.

## Cat reference visuals

Lakitu and Rio now load saved photo-guided prefabs under Resources/Cats. Meshes, editable materials, coat PNGs, joints and authoring/runtime code are retained in Git; see [cat resource guide](../Assets/Art/Cats/README.md). The supplied archive had photos rather than cat meshes, so these are stylized authored models. Raw references remain outside Unity in ignored PrivateReferences. See [markings and cover provenance](BRANDING.md). Only derived pet art is packaged; no household/person photo backgrounds are copied. A future imported rig can replace the visual child while preserving each original root collider, Rigidbody, CatCompanion and save identity.

## Shared scenery

The retained SceneryPalette connects original materials/textures and reusable furniture, window, fence, foliage, roof and park props. [Scenery resource guide](../Assets/Art/Scenery/README.md) explains edits and rebuilding. Runtime WorldArt preserves the tested solid layout while changing its presentation; ScenePresentation restores the user's original lighting/fog/shadow settings on teardown.
