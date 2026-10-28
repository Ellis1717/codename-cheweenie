# Marty art pipeline

The procedural canine is original placeholder geometry: four animated paws, long body, muzzle, eyes, floppy ears and tail.

Import an owned or licensed FBX into `Assets/Art/Marty/`. Use metres, +Z forward and +Y up. Put a Generic rig and Animator on a prefab with its paws about 0.75 metres below the gameplay root. Adjust the visual child transform to fit the gameplay capsule. Disable root motion. Optional Animator parameters: `Speed` (float), `Cozy` (bool).

Save the prefab as `Assets/Resources/MartyVisual.prefab`. Bootstrap loads it automatically. Visual colliders are disabled; the independent Rigidbody/capsule owns movement. Keep materials and textures inside the project. No art is downloaded by the game.

Create a **Cheweenie / Canine Tuning** asset at `Assets/Resources/MartyTuning.asset` to tune acceleration, speeds, turn rate, jump speed and interaction distance. Test keyboard, controllers and animation after importing. Placeholder proportions are not final Marty likeness.
