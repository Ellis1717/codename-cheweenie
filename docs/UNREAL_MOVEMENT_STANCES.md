# Codename .... Cheweenie — moving jumps, crawl and two-leg walking

The validated **0.15.0 desktop main** includes these transient movement stances for **solo skeletal Marty**. Restart **Play Cheweenie**, then choose **Play / continue adventure**.

| Action | Xbox / PlayStation | Keyboard |
|---|---|---|
| Move and steer | Left stick | WASD |
| Run | L3 toggle, or hold A / Cross | Hold Shift |
| Jump while moving or running | Y / Triangle | Space |
| Toggle crawl | Hold LB / L1, click L3 | Z |
| Toggle two-leg walking | Hold LB / L1, click R3 | U |
| Select a stance from a menu | B / Circle → Tricks menu | C → Tricks menu |

The comfortable exploration defaults retain precise camera response and L3 hurry toggle. Existing room/pet controls remain available; the new stances apply to solo skeletal Marty. Unmodified R3 still changes camera views. Shoulder chords consume their stick clicks without barking or firing the original stick-click action.

Running jumps preserve horizontal movement and steering. A jump tap up to 120 ms before landing can fire on ground contact; menus, focus loss and disconnection cancel queued jumps. Existing jump feature/energy restrictions apply.

Crawl is a moving four-paw low pose, distinct from lying down. Its capsule height drops from 56 to 44 cm and its speed is capped at 45 cm/s. Two-leg walking raises the torso, folds the forepaws and uses the hind paws for support, with a 96 cm collision capsule and 80 cm/s speed cap. The existing rig and fur meshes are reused. Clothes follow the posed spine.

Stance changes require grounded feet. Capsule changes keep the feet at the same height. Expansion requires overhead clearance; a low ceiling can block returning to four paws, running or jumping from crawl. Once clear, run returns to four paws; jumping from crawl also returns to four paws before takeoff. Two-leg mode supports ordinary jumping. The default follow camera lowers for crawl passages and compensates for the taller upright capsule. Home recovery/restart resets transient stance state. Adventure checkpoints and saved pace preferences remain compatible.

The upright gait is an original procedural, playful animation. It is not final motion-captured character animation. Suitable crawl clearances depend on the actual collision geometry of the environment.

## Actual validation

Native Editor and Game compilation and full cook/stage/archive passed. The final standalone release passed **1,043 required packaged assertions**, including **50 movement-stance**, **59 production-button** and **54 comfort/restart** checks. An additional 22 unlock-seed, four private-map and 18 archive round-trip assertions bring the final packaged total to **1,087 selected executions**. Four audio waveform checks, the brick pixel probe, all 43 current payload hashes and the final runtime-log scan passed. Seven fresh captures were visually reviewed. No new portable ZIP was created.

The earlier rendered Editor revision passed 48 checks. Its first fixture attempt had five cascading failures because a crawl toggle occurred before the teleported pawn landed; a grounded wait fixed the sequence. Capture review then revealed that the jacket was removed before its requested frame rendered. The final capture delay fixes that timing, and final packaged images visibly show the outfit. A redundant final Editor runtime was stopped during startup before gameplay; the final 50-check audit ran in the standalone package instead. These earlier runs are separate from the 1,087 final packaged executions.

The local catalog retains all 39 previous entries/manifests and adds the verified 43-file 0.14.2 payload: **40 archives / 3,006 files**. Verification reuses the prior 39-archive baseline; it does not claim a new full rehash of every archive. The round trip uses the immutable 0.14.2 executable at synthetic 120 Hz with isolated current/archive profiles. Ordinary frame settings and owner preferences are unchanged by that acceptance run. The complete catalog was restored afterward.

Synthetic input and spawned collision fixtures are separate from a human physical controller playtest. Physical feel, PlayStation hardware, separate-PC LAN, broad performance and subjective final animation approval remain unverified. Earlier physical signal sweeps observed all 14 standard digital controls; this milestone did not reset or reprogram the controller.

[PR #55](https://github.com/Ellis1717/codename-cheweenie/pull/55) is merged into main at `25901d4`. Its Unreal tree matches the validated build source. The clean local main checkout is `D:/CheweenieValidation/GameplayFurWorktree`; its launcher and the original workspace launcher select the validated 0.15.0 package. The original mixed native checkout, Unity fallback and content-only comparison preview are preserved. Sports-park development remains uncommitted and under validation in its separate checkout; it is not included in 0.15.0. Its eventual integration must retain these merged movement and controller improvements.

## Actual packaged captures

The platform and low beam are temporary fixtures spawned in the real game map, not a new permanent level. The first four captures use a test portrait camera; the next two use the actual default follow camera.

- [Running jump](screenshots/ue5-stances-running-jump.png)
- [Two-leg walk](screenshots/ue5-stances-two-leg-walk.png)
- [Crawling under the low beam](screenshots/ue5-stances-crawl-beam.png)
- [Two-leg jacket alignment](screenshots/ue5-stances-two-leg-jacket.png)
- [Two-leg follow camera](screenshots/ue5-stances-two-leg-follow.png)
- [Crawl follow camera](screenshots/ue5-stances-crawl-follow.png)
- [Updated Xbox control guide](screenshots/ue5-stances-controller-guide.png)

[Build and release record](evidence/ue5-stances-build.json), [final packaged movement report](evidence/ue5-stances-packaged.json), [earlier rendered Editor report](evidence/ue5-stances-editor-first.json), [initial fixture findings](evidence/ue5-stances-initial-fixture.json), [release gate and payload manifest](evidence/ue5-stances-unified.json).

Run the focused acceptance with `TEST_STANCES.ps1 -GameExe <packaged executable> -Render -OutputRoot <fresh folder>`. The complete release runner accepts `-ReleaseVersion Unified-0.15.0`; the finalizer requires all previous suites, fresh stance checks/captures, valid payloads and clean runtime logs.
