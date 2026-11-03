# Codename .... Cheweenie — 0.13.1 release candidate

This candidate brings the merged readable-brick, Marty close-up-coat and configurable-overlay improvements into the standalone core adventure. Earlier distributions and the Unity fallback remain intact.

## Build and accept

Use a clean checkout of current main. From `Unreal/UE5MovementLab`:

```powershell
./BUILD.ps1
./IMPORT_READABLE_BRICK.ps1
./PACKAGE.ps1 -OutputRoot 'D:/CheweenieValidation/GraphicsLabBuilds/UnrealCore-0.13.1'
./TEST_RELEASE.ps1 -GameExe 'D:/CheweenieValidation/GraphicsLabBuilds/UnrealCore-0.13.1/Windows/CheweenieUE5Lab.exe' -OutputRoot 'D:/CheweenieValidation/Readable0131/Package'
```

`TEST_RELEASE.ps1` runs suites sequentially because they exercise persisted preferences and unlocks. It includes the existing core gameplay/audio/same-machine LAN checks and the rendered 16-check brick-world suite. When the source contains `TEST_OVERLAYS.ps1`, it also requires 79 overlay checks plus three restart checks. Optional `-LegacySave` and `-MapFile` arguments exercise retained-save compatibility and the private exterior. These files remain local.

The finalizer requires fresh reports and captures, clean runtime logs, bark/music/mute waveform checks and hashes for every immutable package file. Select `-ReleaseVersion Unified-0.13.1` when calling `FINALIZE_UNIFIED.ps1` directly. The new launcher supports the previous 0.13.0 gate and requires at least 607 packaged assertions for 0.13.1, or 689 when the marker records verified overlays. It rejects unsupported/incomplete gates before starting a game, then verifies payload integrity. The default finalizer version remains 0.13.0 for previous reproduction commands.

Only after acceptance should the primary desktop shortcut select the new package. Copy the owner's real adventure checkpoint only if the destination does not already have one; do not copy acceptance saves. Keep the previous shortcut and package. Inventory archives using the existing index builder, preserve existing identities/hash records, and verify a real archive-return round trip. A portable ZIP must exclude private `LocalGameData`, runtime `Saved` folders and logs.

After acceptance, `ARCHIVE_RELEASE.ps1 -PackageRoot <package> -ZipPath <new zip>` writes only the validated Windows payload and four portable launcher/readme files. It verifies every entry after compression and rejects private/runtime paths, logs and escaping paths. Existing ZIPs are never overwritten.

## Current verification

Eight launcher guard fixtures and five portable ZIP fixtures passed without starting a game. The local archive catalog preserves all 35 previous identities/hash records and adds the retained 0.13.0 core as the 36th choice. Private map settings were copied only when absent in the new folder.

The initial clean Editor/Game builds and full cook/archive passed, but rendered inspection rejected that candidate: its brick alias still referenced an earlier material revision absent from the clean checkout, so walls displayed Unreal's default grid. The original mixed checkout retained that revision and hid the missing dependency. The importer now updates and force-saves the alias, tags its revision and validates the saved parent/textures in a second Editor process. Fresh-process verification passed. The runtime retains the older brick if the new alias has no usable parent; the acceptance suite requires the new parent and tests the actual screenshot's clay palette. The new pixel check rejects the bad packaged capture and accepts the earlier actual brick render.

The first Editor render attempt was stopped during startup while packaging was active; it produced no gameplay result. The first packaged regression was stopped after the visual rejection, and its reports are retained separately. A subsequent rebuild was stopped before cooking to incorporate newly merged main at `474c5a8` (overlays). Final combined native compilation, packaged acceptance, archive return, portable ZIP and desktop activation are pending. Physical controllers, separate PCs and comparative GPU performance remain unverified. This candidate is not an enabled desktop release.
