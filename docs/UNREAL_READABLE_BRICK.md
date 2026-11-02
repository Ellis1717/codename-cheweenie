# Codename .... Cheweenie — readable brick surfaces

This focused pass improves the existing outdoor brick surfaces at pet-camera distance. It retains the 1024-square texture budget and three texture samples, with no per-brick actors, displacement or extra collision. It is a material improvement, not a measured frame-rate optimization or completed environment-art pass.

## Changes

The original procedural maps now have a separate revision with 24 × 8 cm brick courses, 1 cm mortar joints, rounded clay shoulders and 0.38 cm authored relief. Brick colour varies between courses and individual bricks, with fine clay grain. Mortar is rougher than clay. Relief is encoded in DirectX normal data using physical height derivatives rather than a resolution-dependent arbitrary multiplier. Normal maps still cannot change the wall silhouette or produce deep geometric cavities.

The material projects colour, roughness and normals at a common 192 cm world repeat. Normal perturbations follow the selected projection axes and the outward face normal, instead of interpreting the projected normal map in the cube's unrelated mesh tangent basis. This supports opposing wall faces and the existing instanced gable infill. Standard mipmaps/streaming remain enabled. Base colour is sRGB; normal and roughness are linear.

The separate `/Game/Adventure/Scenery/ReadableBrick/Brick_World` material instance points to a hash-named parent and textures. The neighbourhood uses it when available, retaining the original brick as a fallback. Earlier materials, maps, comparison preview, pets and Unity remain intact. Existing buildings/collision, keyboard and Xbox/PlayStation bindings are retained.

## Reproduce

From `Unreal/UE5MovementLab`, run:

```powershell
./IMPORT_READABLE_BRICK.ps1
./BUILD.ps1
./TEST_WORLD.ps1 -Render -ReadableBrick -OutputRoot "$PWD/Saved/ReadableBrick/Rendered"
```

The generator uses Python's standard library, including a small PNG writer, and the Python executable bundled with Unreal. No paid assets or added Python packages are required. The old full world generator is independent; rerun the brick import after regenerating the world. The dedicated importer retains earlier hash-named revisions instead of deleting loaded material graphs.

The extended world smoke checks that headquarters uses the revision and captures `WorldBrickDogHeight.png` from a scripted camera approximately 57 cm above the outdoor ground. This is a rendered view of the actual wall, not Marty’s live camera. The existing `WorldDogView.png` is the live player camera; the suite also traverses the room using synthetic keyboard input and switches camera using synthetic gamepad input. Physical controller handling remains unverified.

## Verification

Original map generation and Unreal import passed. The import report confirms three 1024-square maps, expected colour spaces and world-space normal output. Native Editor compilation passed on the current checkout, including concurrent pre-existing coat/UI work. The rendered real-map suite passed all 16 assertions, including synthetic keyboard traversal and gamepad camera switching. Runtime Error/Fatal/material fallback matches were zero. The dog-height close-up and headquarters overview were visually inspected; all five 1920 × 1080 captures were produced. Packaged desktop launchers have not been updated. No clean-branch build, physical-controller check or before/after GPU benchmark is claimed.

[Actual close-up](screenshots/ue5-readable-brick-closeup.png) · [Actual headquarters view](screenshots/ue5-readable-brick-home.png) · [Evidence](evidence/ue5-readable-brick.json)

The normal-space implementation follows the distinction in [Epic’s world-aligned texturing documentation](https://dev.epicgames.com/documentation/unreal-engine/texturing-material-functions-in-unreal-engine). Broad performance and subjective art approval require further review.
