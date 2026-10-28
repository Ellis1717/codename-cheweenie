# Validation and Windows deployment

## Actual results — 2026-10-28

- Unity 6000.3.10f1 batch startup was attempted against this project. It exited **198**, before importing or compiling: **No valid Unity Editor license found. Please activate your license.** The installed editor could not perform license activation in this session.
- `dotnet build Validation/Compile.csproj`: runtime scripts compiled against installed Unity 6.3 engine assemblies, targeting .NET Standard 2.1, zero warnings and errors.
- `dotnet build Validation/UnityTestsCompile.csproj`: runtime scripts, editor build utility, EditMode tests and PlayMode test compiled against installed Unity/NUnit/TestRunner assemblies, zero warnings and errors.
- `dotnet run --project Validation/QuestChecks.csproj`: **13 checks passed** for quest ordering, duplicate activation, Cozy requirement, completed-state stability and saved-stage clamping. These checks run outside Unity.
- `Validation/ManifestChecks.ps1`: all declared package versions match the editor's bundled packages; asset metadata GUIDs are unique and assembly definition JSON parses. Legacy input is part of the editor and requires no separate package. This source audit does not run package resolution.

Assembly compilation does not run Unity's asset import, package resolver, scene, renderer, physics, input, audio or serialization. No Unity Test Runner result, gameplay screenshot, controller test or playable Windows build is claimed. M1–M5 have source implementations; their gameplay acceptance checks remain pending.

## Run after activating Unity

Use Unity 6.3 LTS; 6000.3.10f1 is installed on the development machine. The project version requests the 6000.3 LTS family. Resolve manifest packages, and set **Active Input Handling** to **Input Manager (Old)** or **Both** if necessary, restarting when prompted.

1. Run **Cheweenie > Validate project configuration**.
2. In **Window > General > Test Runner**, run all EditMode tests and the PlayMode smoke test. The smoke test checks duplicate bootstrap, floor settling, all four missions and both companion quests. It disables persistence during its run and restores saved data afterward. It does not simulate hardware controls.
3. In an empty scene, press Play. Walk, sprint and jump using keyboard; verify no airborne repeat jump. Test the backyard/entry transition, ramp ascent/descent without jumping, upstairs floor seams and camera near walls. Visit cats in order, sniff the backyard, and verify E cannot complete the fort while C can.
   First verify the cover/title screen at 1920x1080, 1280x720 and 4:3. Ensure the HUD stays hidden and gameplay remains frozen. Open title Settings, edit options, close it and confirm the title remains paused. Start/Continue must release gameplay only after the menu selection button is released. Test How to Play/About/Back, New Adventure confirmation, Return to Title, and Quit in a standalone build. The PlayMode smoke test now checks title/settings/start pause transitions before its world checks; it has been compiled but not executed.
4. Confirm distinct cat resting/patrolling, friendship cooldown, optional quest completion and Lakitu following without walking through furniture. Navigation stops at obstacles; it is not a pathfinding system.
5. Enter Cozy at the blanket, then leave with movement/jump. Approach the gap between the two cushion placeholders and press B/LB. Confirm squeeze motion, chime and optional particles. Verify the pause menu freezes both gameplay and its timers.
6. With real Xbox and PlayStation controllers, test all actions. Use Escape/Start settings to select a controller preset, then rebind individual buttons if the driver maps them differently. Navigate with stick, activate with raw button 0/Enter, and verify changes survive a restart. Escape always opens/closes the menu and cancels capture. Movement axes use standard left-stick axes 1/2 with a 0.2 dead zone; axis rebinding is not implemented.
7. Change volume/camera/particles, complete missions, quit and relaunch. Check stage, friendship, companion objectives and bindings restore. New Game requires selecting confirmation a second time and preserves settings. Save data uses versioned JSON in Unity PlayerPrefs; character position and transient animation state are intentionally reset at startup.
8. Check Console for errors and capture screenshots only after a successful play session. Retain real test XML/logs as evidence, without committing generated Unity folders.

## Batch commands (PowerShell)

From the project root, with an activated license:

```powershell
$unityExe = 'C:\Program Files\Unity\Hub\Editor\6000.3.10f1\Editor\Unity.exe'
& $unityExe -batchmode -nographics -projectPath $PWD -runTests -testPlatform EditMode -testResults TestResults/edit.xml -logFile edit.log
& $unityExe -batchmode -nographics -projectPath $PWD -runTests -testPlatform PlayMode -testResults TestResults/play.xml -logFile play.log
& $unityExe -batchmode -quit -projectPath $PWD -executeMethod CodenameCheweenie.Editor.CheweenieBuild.BuildWindows -logFile build.log
```

For assembly checks with a different installation, pass `-p:UnityEditorPath='C:\path\to\Editor'`. The optional UnityTestsCompile harness also requires the bundled 3D template TestRunner assembly at its documented reference path; adjust it if your installation differs.

## Windows build

Install Windows Build Support in Unity Hub. Save all open scenes, then choose **Cheweenie > Build Windows prototype**, or run the batch command above. The utility validates legacy input configuration, creates an empty bootstrap scene under ignored `Assets/Generated`, restores the editor's scene setup, and builds `builds/Windows/Cheweenie.exe`. Save a scene once before using the menu command; it refuses unsaved scenes/changes. Distribute the entire Windows output folder, including the Data directory and UnityPlayer DLL. Keep development build output out of Git.
