# Bork or Treat — Halloween special (0.11.1)

Version 0.11.1 corrects Marty's lengthwise hot-dog attachment; see the [fit results](evidence/ue5-hotdog-fit-build.json). Reopen the Bork or Treat desktop shortcut for this patch.

An optional solo festival for **Codename .... Cheweenie**. Choose **Halloween special / Bork or Treat** from Title, Pause or Controls. The separate **Play Cheweenie Bork or Treat** desktop launcher opens this menu directly. Select **Start / continue** to go outside.

Visit eight lantern porches, collect their paw-print stickers, and return the full paw-pass to headquarters for the permanent **Pumpkin Pal** badge. Each doorstep has an original silly greeting, including Fox Ellis and Leo Ellis. Use the existing map/compass Objective entry to find the nearest unvisited porch. E / Xbox X / PlayStation Square interacts; WASD / left stick walks; mouse / right stick looks; Esc / Options pauses. H recovers Marty home and can also reach the indoor finale.

The special has an original plum-and-amber menu illustration, glowing smiling pumpkins, lanterns, porch pennants, nearby labels and gentler outdoor light. Optional hot-dog, pumpkin and bee outfits are in its costume menu. Outfits are never changed automatically. Bring Lakitu and Rio along through Crew (K / D-pad Down), then Call both cats.

## Editable assumptions

- No timer, scares, combat or required date. The event can be played any time.
- Pets collect stickers rather than eat human candy. Normal food, rest and other pet actions remain available.
- Eight public, fictional porch names correspond to the existing eight neighbourhood house slots. They are not claims about real residents. Private street layouts and neighbour names remain local.
- The event starts disabled on new and older saves. Its stamps, active preference and reward are additive saved fields; Cozy Patrol and Ratchet progress remain separate.
- Pause the event to hide decorations and restore the ordinary objective. Stamps remain. **Restart sticker trail** clears only that round's stickers/completion; the badge and other earned rewards remain.
- Normal patrol collection/progression pauses while the event is enabled. Existing life, door, recovery and companion interactions remain usable.
- This doorstep event is solo. The existing LAN pet selection and replicated optional costumes remain available through the normal launcher; doorstep stamps are not networked.
- Poster pets are an illustration. Gameplay retains the current stylized meshes and animation; this update does not claim photorealistic fur or a new feline rig.

Porch positions/names/greetings and decoration tuning live in `HalloweenSpecial.cpp`. Menu behaviour lives in `HalloweenMenu.cpp`; the native HUD supplies text, buttons and saved progress rather than baking UI into the art. Source image and prompt provenance are retained under `SourceArt/HalloweenSpecial/`. Original cover, preview, Unity fallback and previous packages are preserved.

## Rebuild and verify

Run `BUILD.ps1`, `IMPORT_FESTIVAL.ps1`, then `PACKAGE.ps1 -OutputRoot <new-folder>` in `Unreal/UE5MovementLab`. Run `TEST_FESTIVAL.ps1 -GameExe <new-folder>/Windows/CheweenieUE5Lab.exe -OutputRoot <reports>/Festival -Render`, the regression checks described by the release finalizer, and `FINALIZE_FESTIVAL.ps1 -PackageRoot <new-folder> -TestRoot <reports>` before launching. Acceptance saves are isolated from the player's checkpoint. Test fixtures position the pawn, set unrelated progress and use synthetic keyboard/controller input; physical device tests are separate.

Actual release results are recorded in `evidence/ue5-festival-build.json` and README after validation.
