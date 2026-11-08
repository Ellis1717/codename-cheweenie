# Codename .... Cheweenie — community sports park

The neighbourhood park has a connected sports area with a soccer pitch, baseball diamond and basketball half-court. This integration extends the validated cumulative 0.15.0 source, preserving moving jumps, crawl, two-leg walking, controller comfort, jackets, upstairs rooms, digging, Rainbow Guy, natural pet voices and the other released activities. Separate experiments in the original mixed checkout remain separate until integrated and verified.

Visit the park on the walking tour and follow the sports sign/path. The map adds **Park soccer pitch**, **Park baseball diamond** and **Park basketball court** after existing destinations, preserving saved destination indices.

| Activity | How to play |
|---|---|
| Soccer | Approach the ball, face either goal and press **E / Xbox X / PlayStation Square**. Chase it and kick again. A goal crossing adds one goal; an out-of-bounds ball returns to centre. |
| Baseball | Interact at the tee, then touch **1 → 2 → 3 → HOME** within 25 seconds. **Shift / L3** hurries using the existing controller setting. First base is on Marty's right when facing the outfield. Skipped bases do not count. |
| Basketball | Approach the orange ball, face the hoop and interact to shoot. Aimed shots score, misses do not; the ball returns to the shooting spot. |

Each sport has a labelled ball-reset bollard. Resets keep scores; the entrance scoreboard and success messages show the totals. Scores last until the game closes and do not modify adventure progression. Pause freezes the games. Existing home recovery remains available.

This is an original, dog-scale practice park with simple scripted ball motion. Its layout is fictional, including when a retained private street map supplies its anchor. It does not establish surveyed boundaries or real-world sports facilities. Team AI, competitive rules, pitching/outs and outdoor multiplayer are outside this first practice-game scope.

## Release verification

**Desktop main 0.15.1 is active (2026-11-06).** Restart **Play Cheweenie**, choose **Play / continue adventure**, and follow the park sports path. All three main launcher junctions and the desktop shortcut were verified. The checkpoint, settings, private map and dog roster match the previous release. The original mixed checkout, Unity and comparison preview remain intact.

Native Editor/Game builds and full cook/stage/archive passed (771 cooked packages). Final required packaged acceptance passed **1,081 assertions**; seed/private-map/private-sports and the old-release launcher round trip add 70, for **1,151 packaged executions**. Editor sports, stances and controller comfort/restart add 130: **1,281 selected assertion executions**. Six final packaged captures were visually reviewed; four audio waveform checks and brick/coat checks passed. All 43 current and 43 newly archived payload hashes verified. The catalog retains 41 archives / 3,049 files; older entries reuse the previous 40-archive baseline rather than a fresh complete rehash. No new portable ZIP was generated.

An initial home test used an already-upgraded save instead of the required pre-0.13 fixture (23 passed / 11 failed). The retained older read-only fixture passed 34 home + 5 restart checks. Acceptance resumed at Home against unchanged payload, keeping earlier passing reports; finalizer freshness and runtime-log gates passed. This was a fixture correction, not a native gameplay change. [Initial finding](evidence/ue5-park-main-home-fixture.json), [corrected home report](evidence/ue5-park-main-home.json).

All gameplay input was synthetic. Human physical gameplay, PlayStation hardware, separate-PC LAN, broad performance and subjective final animation approval remain unverified. [Build record](evidence/ue5-park-main-build.json), [release gate](evidence/ue5-park-main-unified.json), [activation](evidence/ue5-park-main-activation.json).

Actual packaged views: [soccer](screenshots/ue5-park-main-soccer.png), [baseball](screenshots/ue5-park-main-baseball.png), [basketball](screenshots/ue5-park-main-basketball.png). These elevated test cameras hide the HUD; normal play keeps its existing camera and configurable HUD.

`TEST_PARK_SPORTS.ps1 -Render` checks the real Editor game map. Add `-GameExe` for a standalone package. `-MapFile` supports a headless private-map audit and rejects rendered private-layout captures. Tests isolate their save/profile data and use synthetic keyboard/gamepad input; physical controllers require separate human checks.

The release runner and finalizer retain the 0.15.0 requirements, including 50 movement-stance checks, and add fresh sports/neighbourhood reports and three 1280×720 sports captures. The launcher and portable-archive gate require both the movement-stance and sports verification flags for 0.15.1. Source is `ParkSports.h/.cpp`, with neighbourhood interaction and appended navigation integration.
