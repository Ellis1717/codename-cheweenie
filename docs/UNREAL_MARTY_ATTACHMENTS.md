# Marty’s connected ear and tail skin

Version 0.11.2 addresses the gaps visible in the owner’s gameplay capture. The old ears and tail were separate closed surfaces with shallow intersections. Their component-space animation also left the ear roots behind when the head tilted.

Both playable coat variants now use a single closed torso/head/ear/tail skin. The generator buries the roots before a 0.32 cm voxel union, interpolates and normalizes the original skin weights, and blends the tail into the spine. Upper-skull weights follow the head; the ear-root colour transitions into the darker ear. Ear bases inherit the animated head, tips inherit their ear base, and the tail inherits the spine before secondary flop/wag is added. The original 24 reference joints and their bind matrices are unchanged.

The new assets and Blender/FBX sources live in sibling `MartyConnected` and `MartyConnectedRealism` folders. Original models, textures, generators, the independent preview, Unity and earlier packages remain retained. The original coat/normal PNGs are copied bit-for-bit. Fine fur remains skin-weighted geometry; its own section no longer casts shadows, reducing close-camera Virtual Shadow Map work while the underlying body continues casting shadows. There is no new groom, cloth simulation or photogrammetry claim.

Close-up testing also exposed a non-Nanite Virtual Shadow Map marking-queue overflow. The project now starts directional clipmaps at level 9 and uses directional/local resolution biases 2/3. This trades excess shadow resolution for a bounded near-camera workload while retaining dynamic body shadows. A smaller bias-only adjustment still overflowed; the final settings passed the close-up probe with on-screen warnings left enabled. Broad performance and other hardware remain unmeasured. The release finalizer rejects VSM overflow log matches as well as runtime errors and material fallbacks.

## Reproduce

From `Unreal/UE5MovementLab`, using Blender 5 and Unreal 5.8.4:

```powershell
& 'C:/Program Files/Blender Foundation/Blender 5.0/blender.exe' --background --factory-startup --python Scripts/build_marty_connected.py
& 'C:/Program Files/Blender Foundation/Blender 5.0/blender.exe' --background --factory-startup --python Scripts/build_marty_connected.py -- --detailed
& 'C:/Program Files/Blender Foundation/Blender 5.0/blender.exe' --background --factory-startup --python Scripts/audit_marty_connected.py
./IMPORT_CONNECTED.ps1
./BUILD.ps1
./PACKAGE.ps1 -OutputRoot D:/CheweenieValidation/GraphicsLabBuilds/UnrealBorkOrTreat-0.11.2
./TEST_GAIT.ps1 -Render -GameExe D:/CheweenieValidation/GraphicsLabBuilds/UnrealBorkOrTreat-0.11.2/Windows/CheweenieUE5Lab.exe -OutputRoot D:/CheweenieValidation/ConnectedRelease/Gait
```

Import saves the generated skeleton package before the mesh. A mesh with only an in-memory skeleton is rejected. Materials are reused from the retained original assets.

## Verification and limits

The source audit checks one closed primary skin, no boundary or nonmanifold edges, normalized weights, at most four influences, unchanged bind matrices and identical retained texture hashes. Native checks independently compare the five animated ear/ear-tip/tail origins with parent-transformed reference offsets, through walking, trotting, head lowering at the kitchen bowl, body sway, lying, rolling, recovery and live coat switching. Costume and two-process LAN checks also exercise these origins.

Close-up screenshots come from the actual rendered map with positioned camera/pose fixtures. The smooth-coat screenshot is allowed to finish before switching models. Tests use synthetic keyboard/gamepad input; physical controllers, separate computers, subjective likeness and broad performance remain unverified. The cats’ separate static models are outside this Marty repair.

An existing navigation smoke check failed its fixed 0.4-second Up-key window in two batch runs, although an isolated run passed. The fixture now waits for the same five-centimetre displacement, bounded at two seconds, fails if the playing screen changes, and logs distance/time/screen. Gameplay input bindings and movement logic were not changed for this test repair.

See [actual build results and capture links](evidence/ue5-marty-roots-build.json). Reopen **Play Cheweenie Bork or Treat** to select the validated patch; an already running older game is not hot-patched.
