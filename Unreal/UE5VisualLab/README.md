# Codename .... Cheweenie — UE5 graphics preview

Separate Unreal Engine **5.8.4** project. The Unity family game and the original `Unreal/VisualLab` 4.27 project remain separate. This preview uses copied versions of the retained Marty, Lakitu and Rio static meshes and coat materials. It does not contain the Unity missions, saves, pet animation or Marty controller.

## Open and explore

Run `LAB_UE5.cmd` from the repository root, or run `OPEN_PREVIEW.ps1 -Play` here. It starts a 1280×720 editor-game preview; the installed Unreal editor is required. Run `OPEN_PREVIEW.ps1` without `-Play` to edit the room. The default engine location is `D:\Epic Games\UE_5.8`; both scripts accept `-EngineRoot`.

The viewer uses Unreal's default free-camera pawn: W/S or Up/Down move forward/back, A/D strafe, Left/Right turn, mouse looks, E/Space rises and Q/C/Control descends. Gamepad left stick translates and right stick looks; A/Cross rises and clicking the right stick descends. This is a flying room viewer, not a controllable pet. Physical Xbox/PlayStation checks remain a human test. Close the window or use Alt+F4 to exit. The normal Unity game still launches through `PLAY.cmd`.

First startup can compile shaders and distance fields. Wait for compilation before evaluating materials. The project requests DX12, Shader Model 6, software Lumen GI/reflections, distance fields, virtual shadows and temporal super resolution. Hardware ray tracing is disabled. Cache generation is directed to `D:\CheweenieValidation\UE5Cache` by these launchers; use `-CacheRoot` for another drive. Settings are project-local; no global Unity renderer or save settings are changed.

## Retained resources and rebuild

`Content/VisualLab` contains the independently copied/resaved pet meshes, cat coat textures and materials. Original source meshes and metadata remain in `../VisualLab/SourceAssets`; original scenery textures remain in `../../Assets/Art/Scenery`. The generator imports those owned scenery textures and creates its dedicated materials and room. No external artwork or paid dependencies are required.

Run `BUILD_PREVIEW.ps1` to regenerate the **owned Comparison map**. This deliberately replaces actors in that map; put hand-edited scenes in another map. Pet assets are loaded from the copied Content folder and are not reimported or altered at their original paths. The script checks map saving, all three pets and rendering settings; it writes `Saved/preview-report.json` and `Saved/BuildPreview.log`. Saved/cache/intermediate files are ignored by Git.

The room adds a framed open window, floorboards, trim, woven rug, rounded sofa/cushions, side table and planter. Lighting combines daylight, a soft window source and a warm lamp. The geometry is still schematic; it is not a reconstruction of the family's actual room. Lighting does not turn these articulated static pet exports into realistic skinned/groomed characters. Proper anatomy, a connected skin-weighted rig, coat detail and authored movement clips remain the next character-art milestone.

## Verification

Native generation and rendered preview checks are recorded in the main graphics-lab guide after execution. A headless generation pass validates assets/map/settings, not rendered Lumen quality or physical controller operation.

`CAPTURE_PREVIEW.ps1` runs the native offscreen DX12 editor, warms the viewport, captures a 1280×720 game-view screenshot, and exits. Its startup hook is inactive unless the launcher sets `CHEWEENIE_UE5_CAPTURE=1`. It requires a fresh image and a successful editor exit, so a previous capture cannot disguise a failed run. The native render capture also works when desktop capture is unavailable. It does not synthesize keyboard or gamepad input.

Rendering reference: [Epic's Lumen documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/lumen-global-illumination-and-reflections-in-unreal-engine).
