# Codename .... Cheweenie — one core adventure

The main game combines the house, neighbourhood, Cozy Patrol, pet actions, companions, optional costumes, Halloween activities and Ratchet unlock. These are core content, not separate editions to install. **Play / continue adventure** is the primary start-menu choice. Existing progress, feature switches, sound preferences and fur detail remain additive and compatible.

During play, use the house door to explore. Pause exposes **Halloween activities / Bork or Treat**, the wardrobe and companions; the permanent Ratchet unlock still requires finishing Cozy Patrol. Ratchet's memory room is an activity within the same executable. Continuing from the hub preserves an active Halloween event; resuming from pause keeps Marty's location. Low / Medium / High fur settings also apply in the LAN room and remain local to each PC.

## Main menu and controls

**Play Cheweenie** opens the latest validated standalone game. The start menu provides Play/Continue, Host, Join, Graphics & controller guide, Archived versions, Back and Quit. **Pause → Game & versions** opens the same hub. The direct Editor launch retains its familiar title screen with a Game & versions entry.

Arrows / D-pad navigate; Enter / Xbox A / PlayStation Cross select; Backspace / B / Circle go back. Left/right and mouse minus/plus adjust settings. The LAN pause menu has the same settings and version access. Leaving a network room requires confirmation; the prompt explains that a departing host closes the room for both players. Other players keep playing while your menu is open.

Host/Join now work inside the menu. The host page shows local IPv4 addresses and an adjustable port. On the other PC, edit the four IPv4 parts and matching port, then Join. Both PCs need the same current package. These are the existing two-player LAN room features (pet selection, movement, toy, voices and costumes); the neighbourhood and solo quests are not newly networked. Physical controllers and separate-machine networking require owner checks.

## Archived versions

Unity and earlier Unreal builds remain intact. They use different engine runtimes, so a single launcher coordinates their separate executables. Selecting a retained version saves and closes the current game, verifies the selected build's payload, opens it, then returns to the latest hub when that process exits. Quit the archived game normally to return. Historical saves remain with their historical builds; this does not convert Unity saves into Unreal saves or synchronize quests between releases.

`BUILD_ARCHIVE_INDEX.ps1` inventories explicitly supplied local build roots, deduplicates identical file manifests and writes the private `LocalGameData/ArchiveBuilds.json` catalog plus a path-free menu index. It includes only recognized game executables. Existing validation manifests are retained; builds without one receive an integrity snapshot labelled legacy. A snapshot establishes file integrity, not a new gameplay test. A fresh installation lists no archives until its owner supplies retained builds. No paid dependency, downloadable engine installer or source branch switch is needed.

`PLAY_GAME.ps1` accepts only an opaque catalog ID plus a per-session token from the game. The game never writes an executable command. The supervisor checks the selected executable's location, every recorded hash and relative file boundaries before launch. Verification failure returns to the current hub with a short message; details remain in local `HubSaved/ArchiveError.txt`. Only one game runs at a time. Existing builds, resources and launchers are preserved.

## Reproduction

Run `BUILD.ps1`, `TEST_MODE_HUB.ps1` and `TEST_MODE_HUB.ps1 -Lan`; then `PACKAGE.ps1 -OutputRoot <new package directory>`. Run the existing gameplay suites plus the hub suites against its actual executable, using separate report directories. `FINALIZE_UNIFIED.ps1` requires fresh passing baseline and hub reports, rendered captures and clean runtime logs before writing `validated-unified.json` and the launch scripts. Do not fabricate that marker for an untested distribution.

Generate the local catalog with `BUILD_ARCHIVE_INDEX.ps1 -PackageRoot <package> -ArchiveRoots <retained roots>`. For an unattended archive round trip, use a catalog fixture containing a known compatible Unreal release and run `PLAY_GAME.ps1 -PackageRoot <package> -ArchiveAcceptance -TestOutput <directory>`. Test arguments use isolated saves. The historical executable must pass its real 16-check movement suite before the current game reopens and checks its own retained checkpoint and fur setting.

Actual release results and limitations are recorded in README and the linked evidence after verification.
