# Marty’s photo-guided likeness — 0.11.4

This pass uses the owner’s Marty photographs to guide his in-game shape in **Codename .... Cheweenie**. The held front view and seated side view clarify his tapered muzzle, smaller dark eyes, flatter forehead, folded outward ears and compact legs. The warm fawn coat, cream underside, fine check collar and blue bone tag distinguish him from the cats. Clothing is excluded when estimating his body outline. These are inferred authoring proportions from ordinary photographs, without calibrated measurements or a 3D scan. The model remains a procedural approximation.

The detailed model has a longer, leaner torso, lower stance, smaller paws, shorter ears and a less domed head. The eyes sit closer to the face; the nose has a tapered underside and subdued brown colour. A subtle closed lip line replaces the earlier exposed cream chin slab. Short faint whiskers, small claws, a fine alternating collar pattern and a front-facing bone tag complete the pass. Raw photographs and their family backgrounds remain private; only derived artwork and observation records are retained in Git.

## Model and movement

`SourceArt/MartyPhotoReference` retains the Blender scene, FBX, original 4K maps and authoring manifest. The corresponding `Content/Adventure/MartyPhotoReference` folder holds the imported mesh, its own skeleton and dedicated eye/nose materials. Every previous model and generator remains available. Cats, scenery, Unity and the independent preview are unchanged.

The 24 joint names and hierarchy are preserved, but the lower stance and shorter ears deliberately change the reference transforms. The new skeleton keeps those changes separate from earlier assets. Gait now reads the selected mesh’s imported reference pose in component space and refreshes it when switching coats, avoiding an animated-pose cache. Paw lift, stride, body lean, mouth position and action offsets adapt to that reference stance. Continuous skin and parent-relative ear/tail animation remain in place.

The detailed coat keeps 32,000 short fibres and the previous layered directional materials. Single surface-aligned ribbons replace crossed ribbons; outward winding and smooth surface normals remove dark back-facing flecks. The model has 207,678 source vertices versus 339,056 in 0.11.3. This reduces authored geometry; it does not establish a measured frame-time improvement. The source audit verifies one closed primary skin, normalized weights, outward fur faces, the intended new bind pose and unchanged texture hashes.

## Play and rebuild

Reopen **Play Cheweenie Bork or Treat**, then enable **Detailed coat** in Controls to see this model. Turning it off selects the retained smooth connected model. Existing keyboard and Xbox/PlayStation mappings, outfits and save preferences remain supported.

From `Unreal/UE5MovementLab`:

```powershell
& 'C:/Program Files/Blender Foundation/Blender 5.0/blender.exe' --background --factory-startup --python Scripts/build_marty_likeness.py
& 'C:/Program Files/Blender Foundation/Blender 5.0/blender.exe' --background --factory-startup --python Scripts/audit_marty_likeness.py
./IMPORT_LIKENESS.ps1
./BUILD.ps1
./PACKAGE.ps1 -OutputRoot D:/CheweenieValidation/GraphicsLabBuilds/UnrealBorkOrTreat-0.11.4
```

Run all packaged acceptance suites and `FINALIZE_FESTIVAL.ps1` before switching the launcher. The [evidence record](evidence/ue5-marty-likeness-build.json) records actual release checks. The [full-body view](screenshots/ue5-marty-likeness-overview.png), [head](screenshots/ue5-marty-likeness-head.png), [body](screenshots/ue5-marty-likeness-body.png), [meal pose](screenshots/ue5-marty-likeness-meal.png) and [costume](screenshots/ue5-marty-likeness-costume.png) are captures from the real packaged map with test cameras and pose fixtures. They are not concept renders.

Physical controller checks, separate-PC networking, the owner’s likeness approval, subjective listening and broad performance benchmarks remain unverified. Fur uses geometric ribbons and fibre shading; no strand Groom or cloth simulation is introduced.
