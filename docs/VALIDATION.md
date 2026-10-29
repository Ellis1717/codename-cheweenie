# Validation and Windows deployment

## Neighbourhood release 0.2.0 — 2026-10-29

- **46 native Unity EditMode tests and 4 PlayMode tests passed**. New coverage includes all six landmark orders, old-save compatibility, JSON serialization, independent progression, gate/ground geometry, actual quest routing and New Adventure reset.
- The release Windows executable passed **26 acceptance checks**, exit 0, with **zero runtime errors**. Production virtual keyboard/gamepad input walks the complete home/backyard adventures, both Cozy moves, the east garden gate, mailbox, flower garden, park, welcome-home sign and return indoors. Final flags: home 4, backyard 31, neighbourhood 15.
- Final run: **126.58 seconds, 59.80 average FPS at 1280x720**, capped at 60 FPS on this development machine. This is not a cross-hardware benchmark. See [JSON result](evidence/neighbourhood-acceptance.json), [garden screenshot](screenshots/neighbourhood-garden.png) and [park screenshot](screenshots/neighbourhood-park.png).
- **145 existing standalone checks passed**; runtime/editor/test compilation passed with zero warnings/errors. Eleven package versions and 73 metadata entries validated. No additional dependency was needed for the neighbourhood.
- All 139 distributed player files match the tested build. The portable ZIP has 142 entries including the complete runtime and a start guide. The old local release was preserved.

Launch `builds/NeighbourhoodWindows/Cheweenie.exe`, or extract `builds/Codename-Cheweenie-Neighbourhood-0.2.0.zip`. The desktop **Play Cheweenie** shortcut points to this edition. Quit any older running copy before starting the new one so two versions do not overwrite each other's saves. The neighbourhood is available immediately through the east garden gate. Its schematic geography is fictional until family landmarks are supplied. Physical controllers and subjective speaker mix remain human checks.
## Earlier Windows/audio results — 2026-10-28

Unity 6000.3.10f1 now imports and tests this project successfully. Audio validation ran in an isolated copy at `Validation/UnityAudioProject`, with a different company/product name to keep its PlayerPrefs separate from the user's game. The user's running editor was preserved. The repository now includes reproducible Unity 6.3.10f1 settings, a package lock and Both input handling.

- **30 Unity EditMode tests passed**, zero failures. Includes quest progression, backyard rules, bindings and imported audio assets/volume migration.
- **3 Unity PlayMode tests passed**, zero failures. The world smoke test checks title/settings transitions, floor settling, home and backyard quest interactions, ribbon delivery and New Adventure reset. The virtual-device test exercises production keyboard/Xbox/PlayStation input and the Cozy-before-movement regression. The audio test checks scheduled music playback/time advancing on the paused title, menu audio, separate volume controls and home/backyard crossfades.
- **145 standalone checks passed**: 13 home quest, 23 bindings/input gate, 76 backyard and 33 audio. Audio checks cover volume migration/mute, PCM sample safety, loop duration/stereo content and loop-boundary continuity.
- Runtime/editor/test source compilation against installed Unity 6.3 assemblies targeting .NET Standard 2.1 passed with zero warnings/errors.
- Eleven manifest package versions match the bundled editor packages. Asset metadata and assembly-definition JSON are audited by `Validation/ManifestChecks.ps1`.
- Eleven original stereo WAV files were rendered: two forty-second music loops and nine short cues, approximately 7.6 MB total. Generator validates finite, non-clipped, non-silent samples.

The first actual PlayMode run exposed a test setup issue: an interpolated Rigidbody teleport had not updated the rendered Transform before immediate interaction checks. The test now synchronizes both poses. The corrected world smoke and the added audio playback test pass.

Headless tests do **not** verify speaker output, subjective mix quality, rendered menu layout, physical keyboard/controller input or complete manual traversal. The world smoke uses synchronized teleports for quest interactions. A release Windows player and rendered 1280x720 screenshots are now produced separately from the headless test runner.

## Earlier attempts

Before the user's Unity activation/session, batch startup exited 198 because no valid editor license was available. Those attempts did not import or test the project. The successful audio-era results above supersede that blocker.

## Play in the editor

1. Open the project in Unity 6.3 LTS. Let imports and compilation finish.
2. Set **Active Input Handling** to **Both** if necessary; restart when prompted. Run **Cheweenie > Validate project configuration**.
3. Stop any existing Play session before testing changed scripts. Use an empty scene, then press Play and click the Game view.
4. The title should display the cover and play the home theme even though gameplay is paused. Ensure the Game view's Mute Audio control is off.
5. Open Settings. Adjust **Music** and **Sound effects** separately with Left/Right, horizontal stick input, or clicking/activating the row. Music should remain audible when effects are muted; menu clicks and effects should work when music is muted. Closing title settings must leave gameplay paused.
6. Start/Continue. Walk/sprint/jump and listen for soft footsteps, jump and landing cues. Greet the cats; verify distinct stylized chirps. Confirm a single Cozy flourish and Between reward sound.
7. Walk into the backyard: music should fade smoothly to the brighter loop. Return indoors or to title: home music returns. Opening settings softens music; Cozy softens it further. Listen through the forty-second loop boundary for clicks.
8. Finish the original home quest and the backyard adventure in `docs/BACKYARD.md`. Verify walls block interactions. Walk the house/yard ramps, cross loft seams, jump the stones and walk both directions through the hedge tunnel.
9. Test How to Play/About/Back, New Adventure confirmation, Return to Title, and menu layouts at 1920x1080, 1280x720 and 4:3. Verify no menu button leaks into a gameplay action.
10. With Xbox and PlayStation controllers, test all actions and remapping. Driver mappings vary; choose a preset, then rebind as needed. Standard left-stick axes have a 0.2 dead zone; axis rebinding is not implemented.
11. Restart Play and check that progress, friendship, bindings and both volume levels restore. New Adventure must reset world progression/spawns and preserve settings.
12. Check the Console and record any red error with its stack trace. Capture screenshots only from a real play session.

## Automated tests

In **Window > General > Test Runner**, run **EditMode > Run All**, then **PlayMode > Run All**.

Successful headless commands used the editor below with `-batchmode -nographics -projectPath "<isolated-copy>" -runTests`, `-testPlatform EditMode` or `PlayMode`, and explicit `-testResults`/`-logFile` paths. No `-quit` flag is added to test runs; the runner exits itself.

Editor: `C:\Program Files\Unity\Hub\Editor\6000.3.10f1\Editor\Unity.exe`

Local evidence (ignored by Git):
- `TestResults/release-edit.xml`, `TestResults/release-edit.log`
- `TestResults/release-play.xml`, `TestResults/release-play.log`

Standalone commands from the project root:
```powershell
dotnet run --project Validation/QuestChecks.csproj
dotnet build Validation/Compile.csproj
dotnet build Validation/UnityTestsCompile.csproj
```

For a different installation, pass `-p:UnityEditorPath='C:\path\to\Editor'`. The optional UnityTestsCompile harness references a bundled 3D template TestRunner assembly; adjust its path if your installation differs.

## Windows build

Windows Build Support is installed on the development machine. Save all open scenes, then choose **Cheweenie > Build Windows game**. The utility validates input and Windows Build Support configuration, sets the exact product title, creates an ignored bootstrap scene under `Assets/Generated`, restores the editor scene setup and builds `builds/Windows/Cheweenie.exe`. It refuses unsaved scenes/changes.

Distribute the entire Windows output folder, including its Data directory and UnityPlayer DLL. Keep build output out of Git. The build uses Mono and LZ4HC compression. Resources materials explicitly reference world/particle shaders so the player cannot strip dynamically requested shaders. See the built-player acceptance section below.

## Built-player acceptance

The Windows executable supports an explicit opt-in test mode:

```powershell
& '.\builds\Windows\Cheweenie.exe' -cheweenie-self-test -cheweenie-report "$PWD\TestResults\WindowsAcceptance\result.json" -logFile "$PWD\TestResults\WindowsAcceptance\player.log"
```

This creates virtual keyboard/gamepad devices inside the game process, drives the actual Rigidbody through the route, writes JSON and screenshots, then exits 0 on success or 2 on failure. It uses isolated save reads and disables persistence, preserving real player progress. Normal launches do not create virtual devices. Run with a visible window so frame presentation/screenshots are enabled.

Acceptance covers the branded title/settings, keyboard walking/jumping, PlayStation Cross mapping, headquarters ramp ascent/descent, both cats, home quest, Cheeween Cozy and Cheween in Between, backyard landmarks/ribbon delivery, and Return to Title. This is automated device input, not physical hardware testing. **Final result: PASS**, exit 0, 19 checks, 75.34 seconds, 59.68 average FPS at 1280x720 on this development machine. Home stage 4 and backyard flags 31 confirm the full mission completed; runtime errors: zero. The game targets 60 FPS. This is a single-machine capped run, not a cross-hardware performance benchmark. See [JSON report](evidence/windows-acceptance.json), [title](screenshots/title.png), [settings](screenshots/settings.png) and [completed adventure](screenshots/complete.png).

## Play the portable release

Open `builds/Windows/Cheweenie.exe`. Unity does not need to be running. Choose **Start adventure**, use WASD to move, Space to jump, E to greet/sniff, C for Cozy and Escape for settings. Greet Lakitu, greet Rio, sniff the backyard marker, then rest at the blanket fort. The backyard ribbon adventure unlocks afterward. Music and sound effects have separate volume controls.

For another Windows PC, extract `builds/Codename-Cheweenie-Windows-0.1.0.zip` completely before launching. Keep the executable, Data folder, UnityPlayer DLL and Mono runtime together. The current art and house geometry remain prototype placeholders; the title cover is concept art rather than an in-game model preview.
