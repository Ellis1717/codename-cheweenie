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

LAN retains its existing room scope. The shared pet room retains its ball and voices; outdoor quests, upstairs progression, Rainbow Guy and jackets are not newly replicated. Unity's field guide, rebinding presentation and articulated cat motion have not been established as matching native Unreal implementations. Preserve their resources and treat these as explicit parity work. Unity saves are not converted.

`TEST_RELEASE.ps1` runs the standalone integration suites, including rendered life/dig/toy, upstairs, jacket, device-guide and style checks. `FINALIZE_UNIFIED.ps1 -ReleaseVersion Unified-0.14.0` requires fresh reports and captures, recorded squeak/audio checks, the existing solo/LAN/save regressions and clean runtime logs. The launcher verifies payload hashes. Physical hardware and separate-PC checks remain distinct from automated input and local two-process LAN tests.

The validated 0.14.0 package is now the desktop and root-launcher default. Native Editor/Game builds and full cook/stage/archive passed. Validation passed 824 required package assertions, 22 additional unlock-seed assertions, four private-map assertions, 16 Editor movement assertions and an 18-assertion archive round trip: 884 selected executions. Inputs were synthetic; the LAN checks used two processes on this PC. Ten fresh captures were reviewed. Physical controllers, separate PCs, foreground desktop clicks and broad performance remain unverified.

All 43 main payload hashes and 2,877 files across 37 archives verified. All 36 previous archive identities remain, with 0.13.1 added. A catalog regression confirms that a junction to the current payload is excluded from archives. The portable ZIP verified all 47 entries and excludes private map data, saved progress and archive paths. Its PLAY_GAME.cmd opens the combined game without the Editor.

The prior main checkpoint and preferences were copied with matching hashes, along with the retained local map and dog roster. Existing new-package progress would not be overwritten. The prior shortcut and root launcher are backed up; original uncommitted source was not replaced. Source integration is [PR #52](https://github.com/Ellis1717/codename-cheweenie/pull/52). [Full actual results and limits](evidence/ue5-combined-main-build.json), [main menu](screenshots/ue5-combined-main-hub.png), [Rainbow Guy](screenshots/ue5-combined-main-rainbow.png), [upstairs](screenshots/ue5-combined-main-upstairs.png).
