# Validation and Windows deployment

## Latest results — 2026-10-28

Unity 6000.3.10f1 now imports and tests this project successfully. Audio validation ran in an isolated copy at `Validation/UnityAudioProject`, with a different company/product name to keep its PlayerPrefs separate from the user's game. The user's running editor and generated settings were preserved.

- **28 Unity EditMode tests passed**, zero failures. Includes quest progression, backyard rules, bindings and imported audio assets/volume migration.
- **2 Unity PlayMode tests passed**, zero failures. The world smoke test checks title/settings transitions, floor settling, home and backyard quest interactions, ribbon delivery and New Adventure reset. The audio test checks scheduled music playback/time advancing on the paused title, menu audio, separate volume controls and home/backyard crossfades.
- **145 standalone checks passed**: 13 home quest, 23 bindings/input gate, 76 backyard and 33 audio. Audio checks cover volume migration/mute, PCM sample safety, loop duration/stereo content and loop-boundary continuity.
- Runtime/editor/test source compilation against installed Unity 6.3 assemblies targeting .NET Standard 2.1 passed with zero warnings/errors.
- Nine manifest package versions match the bundled editor packages. Asset metadata and assembly-definition JSON are audited by `Validation/ManifestChecks.ps1`.
- Eleven original stereo WAV files were rendered: two forty-second music loops and nine short cues, approximately 7.6 MB total. Generator validates finite, non-clipped, non-silent samples.

The first actual PlayMode run exposed a test setup issue: an interpolated Rigidbody teleport had not updated the rendered Transform before immediate interaction checks. The test now synchronizes both poses. The corrected world smoke and the added audio playback test pass.

Headless tests do **not** verify speaker output, subjective mix quality, rendered menu layout, physical keyboard/controller input or complete manual traversal. The world smoke uses synchronized teleports for quest interactions. No Windows player build or gameplay screenshot is claimed.

## Earlier attempts

Before the user's Unity activation/session, batch startup exited 198 because no valid editor license was available. Those attempts did not import or test the project. The successful audio-era results above supersede that blocker.

## Play in the editor

1. Open the project in Unity 6.3 LTS. Let imports and compilation finish.
2. Set **Active Input Handling** to **Input Manager (Old)** or **Both** if necessary; restart when prompted. Run **Cheweenie > Validate project configuration**.
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
- `TestResults/audio-edit.xml`, `TestResults/audio-edit.log`
- `TestResults/audio-play.xml`, `TestResults/audio-play.log`

Standalone commands from the project root:
```powershell
dotnet run --project Validation/QuestChecks.csproj
dotnet build Validation/Compile.csproj
dotnet build Validation/UnityTestsCompile.csproj
```

For a different installation, pass `-p:UnityEditorPath='C:\path\to\Editor'`. The optional UnityTestsCompile harness references a bundled 3D template TestRunner assembly; adjust its path if your installation differs.

## Windows build

Windows Build Support is installed on the development machine. Save all open scenes, then choose **Cheweenie > Build Windows prototype**. The utility validates legacy input configuration, sets the exact product title, creates an ignored bootstrap scene under `Assets/Generated`, restores the editor scene setup and builds `builds/Windows/Cheweenie.exe`. It refuses unsaved scenes/changes.

Distribute the entire Windows output folder, including its Data directory and UnityPlayer DLL. Keep build output out of Git. A standalone build still needs to be run and verified; it has not been produced as part of audio validation.
