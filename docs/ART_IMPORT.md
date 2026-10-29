# Marty art pipeline

The default Marty character now uses the original `Marty_Explorer.obj` supplied in the owner's archive, imported into `Assets/Resources/MartyVisual.prefab`. Both OBJ and GLB, derived Unity meshes/materials, animated joint hierarchy and reproducible importer are retained in Git under `Assets/Art/Marty` and the editor/runtime scripts. A separately authored photo-guided model is retained as an alternative. See [resource provenance and editing](../Assets/Art/Marty/README.md). The old primitive canine is only the missing-prefab fallback.

Import an owned or licensed FBX into `Assets/Art/Marty/`. Use metres, +Z forward and +Y up. Put a Generic rig and Animator on a prefab with its paws about 0.75 metres below the gameplay root. Adjust the visual child transform to fit the gameplay capsule. Disable root motion. Optional Animator parameters: `Speed` (float), `Cozy` (bool).

Save the prefab as `Assets/Resources/MartyVisual.prefab`. Bootstrap loads it automatically. Visual colliders are disabled; the independent Rigidbody/capsule owns movement. Keep materials and textures inside the project. No art is downloaded by the game.

Create a **Cheweenie / Canine Tuning** asset at `Assets/Resources/MartyTuning.asset` to tune acceleration, speeds, turn rate, jump speed and interaction distance. Test keyboard, controllers and animation after importing. The retained model remains a stylized likeness, with unseen anatomy inferred.

## Cat reference visuals

Lakitu and Rio use separate photo-guided CatVisual profiles rather than generic gray/cream capsules. The raw references are kept locally outside Unity in ignored PrivateReferences/Cats. See [current markings and cover provenance](BRANDING.md). Only derived game art is packaged; no household/person photo backgrounds are copied into the game. Custom rigged cat art can later replace the procedural visual child while preserving each original root collider, Rigidbody and CatCompanion behavior.
