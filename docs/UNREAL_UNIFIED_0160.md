# Codename .... Cheweenie cumulative game 0.16.0

This integration combines sports PR #56, input-aware directions PR #57 and terrain PR #58 with the compatible native features retained in the development checkout. The candidate remains separate from desktop main 0.15.1 until combined packaging and acceptance pass. Source and original resources remain preserved.

## Features and configuration

| Feature | Where to find it | Configuration and scope |
|---|---|---|
| Core adventure and home progression | Play / continue adventure | Existing pet feature switches, upgrades, recovery and saved patrol progress remain. |
| Soccer, tee baseball and basketball practice | Walk to the neighbourhood park | Controls → Pet feature switches → Park sports activities; disabling freezes activity and suppresses interaction, keeping the terrain and session scores. |
| Hills and contoured fields | Sports park | Core world geometry: grassy viewing hills, field crowns, a baseball mound and a level raised court with walkable approaches. |
| Input-aware signs and directions | Controls, Help, gym/door signs and gameplay prompts | Auto / Xbox / PlayStation / keyboard / generic display selection; input preference does not change gameplay bindings. |
| Six camera views and zoom | V / R3; wheel, held +/− or numpad +/− | Per-view zoom now saves for all six presets using controller comfort preferences. Backslash resets the current view; Z remains crawl and U remains two-leg walking. Room D-pad pet selection and RB + D-pad zoom remain. |
| Controller comfort and movement stances | Controls → Controller comfort setup; Tricks menu | Existing dead zones, response/speed, L3 mode, moving jumps, crawl and two-leg walking retained. Stances remain solo Marty features. |
| Character bios | Main Game & versions, pause/title and companion menus → Meet the crew | Marty, Lakitu and Rio pages with retained artwork and owner-established canon. |
| Illustrated inlaid menus | Title and calm utility menus | Controls → Camera, graphics & sound → Illustrated menu backgrounds. In-game menus retain world context; dense controls/maps retain backing for readability. |
| Pet clothing and fantasy styles | Backpack → Clothes on / off | Three independent clothing switches and three outfit selectors; Marty jackets remain a separate selector. Jacket/costume exclusivity prevents stacking. New styles default to classic for older saves. Fantasy selections apply in solo; LAN retains its existing costumes. |
| Lakitu's Hockey Bag Surprise | Game & versions → Lakitu's Hockey Bag Surprise | Optional replayable story; enable/disable under Pet feature switches. Turning it off during play returns Marty home. Uses a scene cut, not driving or skating simulation. |
| Close-up coat update | Marty; Low/Medium/High fur setting | Retains the photo-guided mesh/rig and earlier revisions; retains the latest compatible mesh bindings and material sources. Owner likeness approval and measured performance remain separate. |
| Rainbow Guy, downward digging, upstairs and cozy actions | Existing house, toy, yard and Tricks interactions | Existing controls and applicable pet/overlay switches remain; Rainbow Guy keeps his bite squeak. |
| Seasonal activities and wardrobe | Halloween activities / Bork or Treat | Existing optional activity and clothing settings; part of the same game. |
| Cat companions and Ratchet memory walk | Companion menu; Ratchet unlock entry | Existing follow/stay/friendship and unlock rules retained. Cat skeletal articulation remains unfinished. |
| Indoor LAN, overlays and audio | Game & versions; Controls | Existing host/join, saved overlay controls/styles, fur detail, natural voices and audio settings retained. Outdoor quests/sports are not newly networked by this integration. |

Saved feature IDs are append-only. Sports, hockey and menu artwork add new bits after home progression; old disabled-feature bits retain their meaning. `FantasyCostumeMask` is additive. The first four saved camera distances keep their keys; two new keys provide overview and dog-level defaults. Optional modes do not replace the solo checkpoint.

## Release gates

The native Editor/Game build, full cook/stage/archive, complete packaged regressions and new configuration/restart, camera, crew and hockey suites must pass before activation. Existing sports/terrain, controller, stance, jacket, upstairs, coat, digging, seasonal, audio, save and LAN checks remain required. Actual rendered menus and gameplay must be reviewed, with synthetic input distinguished from physical hardware.

Publication must verify payload hashes, retain the previous package/archive catalog and copy the latest owner checkpoint/settings/map/roster without altering the previous release. The main desktop shortcut and supported main launchers must point at the same validated cumulative package. Older builds remain historical fallbacks.

This work does not complete the planned campaign, articulated cats, outdoor co-op, physical controller gameplay, separate-PC networking or hardware-qualified performance. The [version 1 roadmap](VERSION_1_ROADMAP.md) retains those gates. Unity and the content-only comparison preview remain intact; useful Unity-only behavior still needs deliberate native implementation.

## Actual results

Combined validation and publication are in progress. This section will be replaced with the actual results and release status.
