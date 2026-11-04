# Codename .... Cheweenie — controller button audit

This page records the historical 0.14.1 layout. Current main is 0.14.2, with L3 hurry, stick menu navigation, zoom and saved tuning: [current controller guide](UNREAL_CONTROLLER_COMFORT.md).

This work targets the cumulative main Unreal game, based on the validated 0.14.0 release. The original development checkout also contains later camera/menu experiments; those are not automatically part of this release branch.

## Expected controls

| Xbox / PlayStation | Solo adventure | Menus | LAN room / memory walk |
|---|---|---|---|
| Left stick | Move, with analog speed | No movement | Move |
| Right stick | Orbit camera; body sway when selected | No camera movement | Orbit camera |
| A / Cross | Hold to hurry | Select/confirm | Hold to hurry; confirm when menu open |
| B / Circle | Open Tricks | Back | Back in room menu |
| X / Square | Interact, pick up or bury toy | No gameplay interaction | Pick up shared toy |
| Y / Triangle | Jump | No jumping | Jump |
| LB / L1 | Short tap: bark; hold: trick layer | No bark/tricks | Voice |
| LB + A / L1 + Cross | Lie down / stand up | Confirm without a queued trick | No trick layer |
| LB + B / L1 + Circle | Roll | Back without a queued trick | No trick layer |
| LB + X / L1 + Square | Shake paw | No gameplay interaction | No trick layer |
| LB + Y / L1 + Triangle | Bark | No jumping | No trick layer |
| RB / R1 | Recenter camera | Recenter remains available | Recenter camera |
| LT / L2 | Drop toy | No dropping | Drop shared toy |
| RT / R2 | Dig at a patch | No digging | Unassigned |
| Right-stick press / R3 | Cycle release camera views | No cycling | Cycle camera views |
| Left-stick press / L3 | Unassigned | Unassigned | Unassigned |
| Menu / Options | Pause | Back/resume | Open/close local room menu |
| View / left special | Backpack | Close backpack when already open | Return to adventure in offline memory walk |
| D-pad Up | Map | Previous row | Toggle costume; previous row in room menu |
| D-pad Down | Companions | Next row | Return home; next row in room menu |
| D-pad Left/Right | Unassigned in this release | Adjust selected setting | Change pet; adjust setting in room menu |

The PlayStation left-special mapping depends on the driver: the guide labels it Touchpad, but an XInput emulation layer can expose Share/Create instead. Console/system, capture and microphone buttons remain platform functions. No system button is reassigned by the game.

## Verification

`TEST_CONTROLLER_BUTTONS.ps1` drives the ordinary Enhanced Input mappings in the real map with synthetic controller events. It has a separate save slot and user directory. It covers every assigned solo button, all four stick movement directions and stopping, both camera directions, idle diagonal noise, all release cameras, repeated recenter, menu navigation/adjustment, gameplay suppression while paused and held-input flushing. Unassigned L3 is checked for accidental effects.

The existing trick-modifier suite covers all four shoulder/face chords, same-frame chords, release behavior, disabled actions and menu disarming. Existing LAN suites cover replicated movement, toy actions, voices, camera changes, pet switching and admission/reconnection behavior. These automated checks do not establish that every physical button or a particular PlayStation driver works.

Native Editor/Game compilation and the full 778-package cook/stage/archive passed. The native headless production-binding audit passed [57/57 checks](evidence/ue5-controller-buttons-editor.json).

The owner performed physical sweeps after reconnecting the USB controller. The passive XInput monitor observed all 14 standard digital controls, both triggers reaching 255, and both stick axes moving in each direction. [Raw signal evidence](evidence/ue5-controller-physical-signals.json). These observations establish hardware signals, while the synthetic runtime checks establish in-game action behavior; they do not replace a human physical gameplay session. The initial capture had no XInput connection; the next sweep omitted the two center buttons/stick clicks, which were captured in the final targeted pass.

If this is the Wolverine Ultimate, its extra multi-function controls follow onboard profile remapping, as described in [Razer's official guide](https://dl.razerzone.com/master-guides/RazerControllerSetupForXbox/WolverineUltimate/WolverineUltimate-en.pdf). The game consumes the resulting standard controller inputs; no onboard remapping was changed.

The desktop shortcut and both main junctions now select **0.14.1**. Restart **Play Cheweenie**, then choose **Play / continue adventure**. The previous 0.14.0 package remains a historical fallback. Checkpoint, preferences and both private local resources still match their prior-main hashes after the launcher checks.

Final validation passed **905 required packaged assertions**, including 57 production-button checks, 16 movement checks, 32 shoulder-chord checks and 144 LAN assertions across the normal/unlocked host/guest runs. The 22 additional unlock-seed, four private-map, 18 archive round-trip and 57 Editor controller assertions bring selected executions to **1,006**. Three current captures were reviewed. All 43 current payload hashes and 2,920 files across 38 retained archives verified. [Full actual evidence](evidence/ue5-controller-0141-build.json), [Xbox guide](screenshots/ue5-controller-0141-guide.png), [gameplay](screenshots/ue5-controller-0141-gameplay.png).

The historical 0.13.1 round-trip fixture initially failed keyboard stopping with live hardware polling, then failed its first keyboard movement check uncapped. Its binary remains unchanged. Acceptance-only `-NoGamepad` and `t.MaxFPS 60` made the synthetic fixture independent of live stick signals and stable in timing; the final 16 archived movement + two return checks passed. Ordinary main/archive play keeps hardware input enabled.

Physical signal observation and synthetic in-game verification remain separate. A human physical gameplay session, PlayStation hardware, separate PCs and comparative performance remain unverified. Unity and the independent content-only preview are unchanged. No new portable ZIP was generated.
