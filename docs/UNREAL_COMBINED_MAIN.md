# Codename .... Cheweenie — cumulative main game

The main Unreal game is the cumulative product. Supported activities, graphics choices and clothes belong in that game; historical executables remain optional backups. Validate accumulated development improvements in a new standalone release before changing the main launcher.

The 0.14.0 integration starts at main `3658cd6` and combines the local Rainbow Guy/squeak, upstairs, downward digging, controller guide and inlaid menu work, plus the completed jacket branch. It retains the merged 0.13.1 materials, natural pet voices, backpack clothes, overlay styles, home progression and previous adventure systems. The original working checkout and archived builds remain intact.

| Option | Access in the combined game |
| --- | --- |
| Core adventure, home and neighbourhood | Play / continue adventure; use the house door or map |
| Cozy Patrol and exploration | Existing patrol progression and exploration controls |
| Bork or Treat | Pause / Controls → Halloween activities |
| Clothes, costumes and jackets | Backpack → Clothes on / off; Controls → Clothes & costumes |
| Ratchet memory walk | Existing title/pause option after the Cozy Patrol unlock |
| Two-player LAN pet room | Game & versions → Host / Join |
| Fur, overlays and sound | Graphics / Controls; saved local preferences |
| Controller illustration | Controls → Controller display; Auto and manual options |
| Historical releases | Archived versions; separate runtimes and historical saves |

LAN retains its existing room scope. Outdoor quests, upstairs progression and jackets are not newly replicated. Unity's field guide, rebinding presentation and articulated cat motion have not been established as matching native Unreal implementations. Preserve their resources and treat these as explicit parity work. Unity saves are not converted.

`TEST_RELEASE.ps1` runs the standalone integration suites, including rendered life/dig/toy, upstairs, jacket, device-guide and style checks. `FINALIZE_UNIFIED.ps1 -ReleaseVersion Unified-0.14.0` requires fresh reports and captures, recorded squeak/audio checks, the existing solo/LAN/save regressions and clean runtime logs. The launcher verifies payload hashes. Physical hardware and separate-PC checks remain distinct from automated input and local two-process LAN tests.

Build and gameplay results are pending. This document does not mark an untested package as released.
