# Codename .... Cheweenie — controller comfort

The Windows device description confirms **Razer Wolverine Ultimate**. This update builds on the cumulative 0.14.1 game. The existing keyboard, Xbox and PlayStation actions remain available, with extra controller shortcuts and saved tuning.

## Game layout

| Input | Action |
|---|---|
| Left stick | Analog movement; navigate menus with a deliberate deflection |
| Right stick | Camera look; the existing body-sway mode retains its own movement behavior |
| L3 / left-stick click | Toggle hurry by default; hold/off modes are available |
| A / Cross | Hold to hurry; select in menus |
| X / Square | Interact, pick up or bury a toy |
| Y / Triangle | Jump |
| B / Circle | Tricks; Back in menus |
| LB / L1 | Quick tap barks; hold with A/B/X/Y for lie/roll/shake/bark |
| RB / R1 | Recenter |
| R3 / right-stick click | Cycle the four release camera views |
| D-pad left/right | Solo: zoom out/in. Rooms: change pet; hold RB for zoom instead |
| D-pad up/down | Solo: map/companions. Menus: previous/next row |
| LT / RT | Drop / dig; digging remains a solo activity |
| View / Menu | Backpack / pause; room behavior retains its existing roles |

Hurry ON appears in the status HUD. Turning off the hurry toggle returns to the selected base pace; an existing Jog/Run pace choice is preserved. Menus, focus loss, disconnection and travel clear the transient hurry state. A stick held during a menu transition must return near center before it can move Marty or turn the camera. Keyboard movement and mouse look retain their independent inputs.

Vertical menu holds repeat after a short delay. Horizontal stick adjustments occur once per deflection, so holding sideways cannot repeatedly flip a checkbox. D-pad and keyboard navigation remain available.

## Saved tuning

Open **Controls → Controller comfort setup**, or **Game & versions → Graphics → Controller comfort setup** in solo/rooms.

[Rendered controller settings](screenshots/ue5-controller-comfort-settings.png).
[Xbox control guide](screenshots/ue5-controller-comfort-xbox.png), [gameplay](screenshots/ue5-controller-comfort-gameplay.png).

- **Comfort:** 20% dead zones, 100% turn speed, a precise response near center and L3 toggle.
- **Fast:** 20% dead zones, 150% turn speed, linear look and L3 toggle.
- Tune each stick dead zone independently from 15% to 35%, camera speed from 50% to 200%, precise/linear look, and L3 off/hold/toggle.
- Zoom is saved independently for each of the four camera views, with 80–650 cm limits. Reset zoom and restore all controller defaults are separate actions.

Preferences live locally in GameUserSettings, independently of adventure checkpoints, and apply after map travel/restarts. The precise camera response retains full deflection and does not change keyboard/mouse input or Marty's movement-speed upgrades.

## Wolverine extra controls

The controller's extra buttons emit their onboard assignments. Its configuration app was not found installed, and no onboard mappings, firmware or other Razer devices were changed.

In the **Razer default profile**, M1 mirrors X, M2 mirrors Y, M3 mirrors A and M4 mirrors B; M5/M6 provide sensitivity controls. This lets you interact/jump or use LB + mirrored face buttons while keeping your thumbs on the sticks. A custom onboard profile can differ. [Razer's official guide](https://dl.razerzone.com/master-guides/RazerSynapseforXbox/WolverineUltimate/en/WolverineUltimate-en.pdf).

## Validation status

Native Editor compilation passed. The real-map comfort audit passed **45 checks**, followed by **nine persisted-preference checks in a separate process**. Inputs were synthetic; focus loss and disconnection were simulated. The audit uses a separate adventure save and user directory.

The menu-resume fixture now holds the stick before pressing Back, matching its stated scenario. The Editor runner explicitly permits writes to its isolated config directory; Unreal's `-Multiprocess` otherwise suppresses config writes and cannot establish persistence.

The two-process room fixture samples each axis once per frame. Its former held-value/zero-release calls in the same frame summed into a retained paired axis, preventing the required neutral menu reset. This correction changes acceptance input generation only.

The legacy movement runner also uses its own user directory: its default-distance assertion must not inherit zoom saved by an earlier room test. The final packaged run retains its already passing controller reports when resuming the remaining suites; the finalizer still requires those reports to be fresh for the current payload.

The uncapped headless unlock fixture initially failed keyboard walking. Its acceptance runner now uses `t.MaxFPS 60`; the final headless and rendered unlock/memory/return checks passed. Ordinary gameplay frame settings remain unchanged.

Native Game compilation, the full 778-package cook/stage/archive and the final native restage passed. The final release gate passed **993 required packaged assertions**, including **45 comfort + nine restart**, **59 production-button** and **176 normal/unlocked LAN** checks. Three final packaged captures were reviewed. The native Editor comfort/restart and two-process room runs passed **143 assertions** in total.

The desktop shortcut and both main junctions now select **0.14.2**. Restart **Play Cheweenie**, then choose **Play / continue adventure**. The 0.14.1 package remains retained. The original mixed development checkout remains intact; this release's integrated source is on `codex/controller-comfort` in the clean validation worktree.

The 22 additional unlock-seed, four private-map, 18 archive round-trip and 143 Editor assertions bring the selected final total to **1,180 assertion executions**. All 43 current payload hashes verified. Checkpoint, preferences and both private files still match the previous main after the launcher checks. All 38 prior archive identities/manifests are preserved; their earlier 2,920-file verification is reused, with the 43 newly archived 0.14.1 files rehashed. The catalog now retains 39 archives / 2,963 payload files. This milestone did not rehash every older archive.

The 0.13.1 and 0.14.1 historical movement fixtures each passed 15/16 at 60 Hz, failing their tight one-frame recenter probe. Their binaries remain unchanged. The final 0.14.1 archive round trip passed 16/16 at 120 Hz, then 2/2 current-game return checks. Frame/input flags apply only to unattended archive acceptance; ordinary play retains the owner's settings. Current-game recentering passed its movement, production-button and room audits.

[Actual build, preservation and test evidence](evidence/ue5-controller-comfort-build.json).

The owner's earlier physical sweep observed all 14 standard digital signals, both triggers and both stick axes. A human assessment of the new layout, new physical gameplay checks, PlayStation hardware, separate PCs and comparative performance remain unverified. No onboard Razer profile or firmware was changed, and no new portable ZIP was generated.
