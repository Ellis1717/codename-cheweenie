# Marty’s close-up coat reference

The owner's November 2, 2026 indoor fur photograph guides this material pass for **Codename .... Cheweenie**: short, dense golden-tan hair lying along the body, occasional darker fibres, richer brown ears and a soft sheen. The photograph is a visual reference, not a calibrated colour sample or a measurement of hair length. The private photograph is not embedded in game assets or committed.

The distance-filtered coat shader uses finer, lower-relief directional marks, restrained broad texture and sparse darker fibres. A warm tint blends continuously toward a richer reddish brown using the existing UV-encoded palette in every surface and hair material. This preserves the welded ear-root colour transition instead of exposing a material-slot boundary. Surface roughness and cloth fuzz are reduced for a softer sheen. These are material changes; the existing geometric hairs, photo-guided body, skeleton, eyes, collar and animation are retained.

`IMPORT_GAMEPLAY_FUR.ps1` creates a new immutable material revision, applies it to Medium and High, and shares the three surface materials with Low. Low retains its hair-free geometry. The Low model's reimporter now resolves the same reference-material folder so a later reimport preserves the update. Older material revisions, models, packages, Unity and the content-only comparison preview remain available.

Reproduce from the repository root:

```powershell
& ./Unreal/UE5MovementLab/BUILD.ps1
& ./Unreal/UE5MovementLab/IMPORT_GAMEPLAY_FUR.ps1
& ./Unreal/UE5MovementLab/TEST_GAIT.ps1 -Render -OutputRoot ./Unreal/UE5MovementLab/Saved/CloseupCoat/Gait
& ./Unreal/UE5MovementLab/TEST_FUR_DETAIL.ps1 -OutputRoot ./Unreal/UE5MovementLab/Saved/CloseupCoat/Detail
```

## Verification

The continuation isolates the coat-only changes onto `codex/marty-closeup-coat`, based on main at `4f0379c`. Clean native Editor and Game compilation, the `gameplay-fur-c6485ebe6901` import and High material link passed. Fresh rendered real-map gait acceptance passed **30 assertions** with synthetic keyboard/gamepad input. Fresh fur-control acceptance passed **15 assertions**, followed by **four Medium restart and four Low restart assertions**, for **53 assertions on the isolated branch**. Final runtime logs had zero Error/Fatal, material-compilation/fallback or virtual-shadow-page-overflow matches.

Four fresh 1280 × 720 Editor game captures from the isolated branch were inspected: [overview](screenshots/ue5-marty-closeup-overview.png), [body detail](screenshots/ue5-marty-closeup-body.png), [gameplay camera](screenshots/ue5-marty-closeup-gameplay.png) and [farther camera](screenshots/ue5-marty-closeup-far.png). They use the existing Medium-coat gait fixture; the separate fur-controls suite verifies live Low/Medium/High selection and persistence. The character remains stylized, and individual hairs filter into the average coat at small screen sizes. [Evidence, final reports and capture hashes](evidence/ue5-marty-closeup-coat.json).

The initial sandbox import stalled at Zen cache startup and was stopped. The authorized rerun caught an incorrect four-slot expectation for Low; Low has three surface slots and no geometric-hair slot, and the check was corrected. Render review caught a boundary from tinting the ear material independently; the final shared palette-based tint removes that slot-specific colour change. A temporary Low asset sharing lock recovered during Unreal's save retry. These findings and the final reruns are recorded in the evidence.

Physical controllers and owner likeness approval remain unverified. The existing desktop package has not been rebuilt by this material pass. The original checkout's unrelated overlay/scenery edits, intermediate material revisions and owner files remain untouched; this continuation prepares a focused PR from the isolated coat branch. Unity was not retested because its source, assets and settings were unchanged.
