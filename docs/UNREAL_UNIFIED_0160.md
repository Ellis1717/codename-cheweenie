# Codename .... Cheweenie cumulative game 0.16.0

This integration combines sports PR #56, input-aware directions PR #57 and terrain PR #58 with the compatible native features retained in the development checkout. The 0.16.0 candidate passed combined packaging and acceptance. Desktop main remains 0.15.1 while its game process is open; the guarded activation is pending its closure. Source and original resources remain preserved.

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

On **2026-11-08**, native UE 5.8.4 Editor and Game compilation passed. Full cook/stage/archive passed; subsequent test-fixture corrections were rebuilt and restaged without changing cooked content. The staged Game executable matches the final native binary byte for byte. Original hockey prop float-literal conversion warnings were nonfatal.

The final payload passed **1,205 required packaged assertions** and **43 payload hashes**, with fresh rendered captures and runtime-log/material checks. Additional final-package runs passed **79 assertions**: private-map traversal 4, private sports 35, headless Ratchet 22, retained 0.15.1 movement 16 and return-to-current-hub 2. Eight final public captures were retained and reviewed, alongside the costume and camera inspections. Bark/music/mute/Rainbow Guy squeak waveforms, coat pixels and readable-brick pixels passed their probes. Earlier selected Editor suites passed 132 assertions across integration revisions; they are separate from the exact final package gate.

Initial audit failures are retained in the evidence. Corrections covered wheel direction changes, six-view input wording, a consistent configuration fixture, hockey input/audio/screenshot timing, four-to-six camera-cycle assumptions, an accidentally expanded cardinal-direction test loop, the moved jacket mouse hitbox and the nine-row hub expectation. The final fresh suite passed all of them.

The retained 0.15.1 executable launched and returned to 0.16.0 through the real archive launcher. The catalog retains **42 entries / 3,092 payload files**. Older archive entries keep the prior verification baseline; the previous main and new main payloads were rehashed. The portable Windows ZIP passed all **47 entry hashes / 43 payload hashes**, excluding saved progress, private maps, rosters and local archive catalogs.

**Desktop activation is pending:** the current 0.15.1 game remains open. The publisher rejected activation before changing files or links, preserving all four owner checkpoint/settings/map/roster files and keeping all three main junctions on 0.15.1. Close that game, then run the retained guarded publication step; copy the latest owner data and verify the shortcut/junctions before claiming desktop 0.16.0. The validated candidate and portable ZIP already exist separately. The rollback/active-game fixture tests passed.

Physical Xbox/PlayStation gameplay, separate-PC networking, measured performance, final pet likeness and version 1 completion remain unverified or unfinished. Unity and the independent content-only preview remain intact.

Evidence: [release summary](evidence/ue5-unified016-release.json), [exact payload gate](evidence/ue5-unified016-validated.json), [source/binary match](evidence/ue5-unified016-source-payload.json), [packaged reports](evidence/ue5-unified016-packaged), [portable archive](evidence/ue5-unified016-portable.json), [publication status](evidence/ue5-unified016-publication.json), [Editor build/results](evidence/ue5-unified016-editor-build.json).
