# Unreal asset/lighting comparison

Separate **Unreal 4.27.2** project; no changes to the working Unity game. This is an asset-import and static-room experiment, not a completed Unreal version of the game.

Open `CheweenieVisualLab.uproject` with the installed Unreal 4.27 editor. The default comparison map is `/Game/VisualLab/Comparison`. The room contains Marty, Lakitu and Rio as static meshes with retained materials/coat textures, floor/walls, lights, a camera and a PlayerStart. Play uses Engine.GameModeBase's default free-camera pawn. WASD/arrows move, mouse/right stick looks, Space/E rises and Q/C/Ctrl descends; left stick translates. These bindings were confirmed in the installed engine's DefaultPawn.cpp; physical controller interaction was not tested.

Shader compilation uses normal asynchronous local workers. Initial opening can take longer; wait for shader compilation to finish before judging materials. Early captures can show white/checker placeholder surfaces. Synchronous compilation was investigated but hit an assertion in the installed legacy engine, so that mode is not enabled in the project. Settings are project-local and do not alter any global Unreal installation or Unity setting.

## Rebuild source assets and room

1. In the Unity lab project, run the editor method `CodenameCheweenie.Editor.VisualLabExport.Export` (batch mode supported). It writes SourceAssets under that project. Copy generated SourceAssets here when exporting from an isolated validation copy.
2. Run `BUILD_COMPARISON.ps1`, optionally providing `-EngineRoot`. It uses the existing 4.27 installation and editor Python plugins; it does not install/download an engine.
3. The script imports three OBJ meshes, creates materials and rebuilds only the dedicated Comparison map. It verifies exactly three pet actors and successful map saving. A report is written under ignored Saved/.

The 4.27 script runner uses a no-space local helper under LOCALAPPDATA to work around its script-filename parser. The full editor startup path supplies the world context needed to spawn actors. No helper protocol or private photos are used. Source exports are owned meshes from retained Unity prefabs, converted from metres/Y-up to centimetres/Z-up. OBJ transfer is static: hierarchy animation, skinning, Unity shaders and scripts are not exported.

The Python API's Rotator constructor uses roll/pitch/yaw order in this version; script rotations therefore use named arguments. Generated Content assets and portable SourceAssets are retained, while Binaries/Intermediate/Saved/DerivedDataCache are ignored.

The default pawn is a viewer, not Marty. The room is not a fair final rendering benchmark yet: it has simple lighting/materials, static actors and no groom. UE5 is a separate migration decision and was not installed for this work. Its Lumen/Nanite features are absent from 4.27. Before a full port, build a new UE5 copy with one high-quality skinned pet, animation, controller interaction and a finished room, then compare with the Unity lab.
