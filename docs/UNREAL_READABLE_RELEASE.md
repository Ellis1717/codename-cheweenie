# Codename .... Cheweenie — core 0.13.1 release

Play Cheweenie now opens standalone core 0.13.1 with the merged readable-brick, Marty close-up-coat and configurable-overlay improvements. Earlier distributions and the Unity fallback remain intact.

## Build and accept

Use a clean checkout of current main. From `Unreal/UE5MovementLab`:

Use a new output folder for each candidate. `PACKAGE.ps1` refuses to overwrite a validated release; its default now targets a candidate folder rather than the archived 0.9.0 build.

```powershell
./BUILD.ps1
./IMPORT_READABLE_BRICK.ps1
./PACKAGE.ps1 -OutputRoot 'D:/CheweenieValidation/GraphicsLabBuilds/UnrealCore-0.13.1-candidate'
./TEST_RELEASE.ps1 -GameExe 'D:/CheweenieValidation/GraphicsLabBuilds/UnrealCore-0.13.1-candidate/Windows/CheweenieUE5Lab.exe' -OutputRoot 'D:/CheweenieValidation/Readable0131Candidate/Package'
```

`TEST_RELEASE.ps1` runs suites sequentially because they exercise persisted preferences and unlocks. It includes the existing core gameplay/audio/same-machine LAN checks and the rendered 16-check brick-world suite. When the source contains `TEST_OVERLAYS.ps1`, it also requires 79 overlay checks plus three restart checks. Optional `-LegacySave` and `-MapFile` arguments exercise retained-save compatibility and the private exterior. These files remain local.

The finalizer requires fresh reports and captures, clean runtime logs, bark/music/mute waveform checks and hashes for every immutable package file. Select `-ReleaseVersion Unified-0.13.1` when calling `FINALIZE_UNIFIED.ps1` directly. The new launcher supports the previous 0.13.0 gate and requires at least 607 packaged assertions for 0.13.1, or 689 when the marker records verified overlays. It rejects unsupported/incomplete gates before starting a game, then verifies payload integrity. The default finalizer version remains 0.13.0 for previous reproduction commands.

Only after acceptance should the primary desktop shortcut select the new package. Copy the owner's real adventure checkpoint only if the destination does not already have one; do not copy acceptance saves. Keep the previous shortcut and package. Inventory archives using the existing index builder, preserve existing identities/hash records, and verify a real archive-return round trip. A portable ZIP must exclude private `LocalGameData`, runtime `Saved` folders and logs.

After acceptance, `ARCHIVE_RELEASE.ps1 -PackageRoot <package> -ZipPath <new zip>` writes only the validated Windows payload and four portable launcher/readme files. It verifies every entry after compression and rejects private/runtime paths, logs and escaping paths. Existing ZIPs are never overwritten.

## Current verification

Clean native Editor/Game compilation and full cooking/archive passed on the combined source (`aa0c56d`, including main at `474c5a8`). The final package passed **689 required native assertions**, including 16 rendered world checks, 82 overlay/restart checks and synthetic keyboard/gamepad controls. A real-pixel brick probe and three bark/music/mute waveform checks passed. Another 22 unlock-seed, four private-map and 18 real archive-return assertions bring the final selected total to **733**; the duplicate copy of the archived movement report is not counted twice. Final runtime Error/Fatal/material-fallback/VSM and cook missing-dependency matches were zero.

Six actual packaged captures were visually inspected, including [brick detail](screenshots/ue5-core-0131-brick.png), [headquarters](screenshots/ue5-core-0131-home.png), [Marty at gameplay distance](screenshots/ue5-core-0131-coat.png) and [overlay settings](screenshots/ue5-core-0131-overlays.png). The wall inspection is a scripted camera approximately 57 cm above ground; the gait and hidden-overlay views use the real player camera. Artwork and pet likeness remain stylized.

All 43 current immutable payload hashes matched. Windows PowerShell 5 launched the retained 0.13.0 core, verified its 16 movement assertions, returned to 0.13.1 for two persistence assertions and restored the complete archive index byte-for-byte. All 2,834 catalogued files across 36 archives verified. The 503,613,128-byte portable ZIP verified all 47 entry hashes (43 Windows payload files plus four launcher/readme files) and excludes private map/catalog data, saved progress and logs. The desktop shortcut was backed up and updated, the prior checkpoint copied with a matching hash, and both private map files retained. [Actual reports, hashes, scope and activation evidence](evidence/ue5-readable-release-build.json).

Eight launcher guard fixtures and five portable ZIP fixtures passed without starting a game. The local archive catalog preserves all 35 previous identities/hash records and adds the retained 0.13.0 core as the 36th choice. Private map settings were copied only when absent in the new folder.

Physical controllers, separate PCs, foreground desktop interaction, owner likeness approval and comparative GPU performance remain unverified. Frame intervals in the world report are offscreen diagnostics, not a before/after benchmark. The original mixed checkout and its unrelated UI/coat resources were not overwritten; Unity source/assets/settings were unchanged and were not retested.

## Repaired findings

The initial clean Editor/Game builds and full cook/archive passed, but rendered inspection rejected that candidate: its brick alias still referenced an earlier material revision absent from the clean checkout, so walls displayed Unreal's default grid. The original mixed checkout retained that revision and hid the missing dependency. The importer now updates and force-saves the alias, tags its revision and validates the saved parent/textures in a second Editor process. Fresh-process verification passed. The runtime retains the older brick if the new alias has no usable parent; the acceptance suite requires the new parent and tests the actual screenshot's clay palette. The new pixel check rejects the bad packaged capture and accepts the earlier actual brick render.

The first Editor render attempt was stopped during startup while packaging was active; it produced no gameplay result. The first packaged regression was stopped after the visual rejection, and its reports are retained separately. A subsequent rebuild was stopped before cooking to incorporate newly merged main at `474c5a8` (overlays). Final acceptance was rerun from fresh reports against the repaired combined package; no results from the rejected package enable the desktop release.
