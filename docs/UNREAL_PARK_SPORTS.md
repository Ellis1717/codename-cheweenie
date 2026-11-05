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

Integration candidate: **0.15.1**. Publication is pending combined native Editor/Game builds, real-map input checks, fresh packaged acceptance, rendered review and preservation checks. Desktop main remains 0.15.0 until those gates pass.

`TEST_PARK_SPORTS.ps1 -Render` checks the real Editor game map. Add `-GameExe` for a standalone package. `-MapFile` supports a headless private-map audit and rejects rendered private-layout captures. Tests isolate their save/profile data and use synthetic keyboard/gamepad input; physical controllers require separate human checks.

The release runner and finalizer retain the 0.15.0 requirements, including 50 movement-stance checks, and add fresh sports/neighbourhood reports and three 1280×720 sports captures. The launcher and portable-archive gate require both the movement-stance and sports verification flags for 0.15.1. Source is `ParkSports.h/.cpp`, with neighbourhood interaction and appended navigation integration.
