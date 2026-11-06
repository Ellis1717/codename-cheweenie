# Codename .... Cheweenie park terrain

The sports park now has a contoured ground surface in development source. Gentle field crowns, a walkable baseball mound, two grassy viewing hills and a raised basketball terrace give the park changes in elevation. The court stays level, with broad sloping approaches. The park perimeter blends into the existing neighbourhood ground.

Soccer has a shallow crown of about 14 cm above its surrounding field grade. The baseball area has a broad 22 cm rise and a 36 cm mound contribution. The court sits about 91 cm above the original park datum. The two viewing hills rise roughly 1.8 m and 2.6 m above the nearby grade, with bench seats near their crests. These are original dog-scale design choices, not surveyed terrain or regulation sports-field measurements.

One colliding triangle surface supplies terrain heights for balls, bases, goals, reset bollards and field paint. Soccer follows the surface and its local slope affects rolling velocity after a kick. A hit baseball cannot sink below the terrain. Basketball shooting uses the raised court's height. Temporary sports scores and adventure saves retain their existing behavior.

Grass uses the retained world material. Field paint follows the contours in short segments, batched as instances without collision or shadows. Terrain vertices use local coordinates to retain precision at the distant outdoor anchor. There are no new paid assets or third-party dependencies.

## Source and validation

Source lives in `Unreal/UE5MovementLab/Source/CheweenieUE5Lab/ParkTerrain.cpp` and `ParkSports.h/.cpp`. The focused suite is `TEST_PARK_SPORTS.ps1 -Render`; it checks field contours, terrain collision, moving/resting balls, scoring, the complete timed baseball circuit, keyboard uphill/downhill travel, synthetic left-stick ramp travel, pause and home recovery. It captures the three fields, an elevated terrain overview and the actual hill follow camera. Private-map audits remain headless.

Native Editor compilation passed. The final real-map tests passed **35 rendered sports/terrain + 16 headless movement + 12 headless neighbourhood checks = 63 assertions**. Five final 1280×720 captures were visually reviewed. The earlier headless private-map sports audit passed 35 checks before the final visual-only cleanup; it is reported separately. Input was synthetic keyboard/gamepad. Physical controllers, comparative performance and standalone packaging of this change remain unverified.

The first rendered build passed 30 checks and failed five traversal checks because its terrain faces were reversed. Clockwise surface winding and a fresh compile corrected the invisible-ground/traversal issue. Later render review removed an overlapping mound skin, fitted field surfaces to the exact ground triangles to prevent grass showing through the clay, and ended the path at the court edge to avoid overlapping surfaces. [Initial finding](evidence/ue5-park-terrain-initial.json), [final rendered checks](evidence/ue5-park-terrain-rendered.json), [build record](evidence/ue5-park-terrain-build.json).

Actual development gameplay: [terrain overview](screenshots/ue5-park-terrain-overview.png), [hill follow camera](screenshots/ue5-park-terrain-follow.png), [baseball field](screenshots/ue5-park-terrain-baseball.png), [soccer](screenshots/ue5-park-terrain-soccer.png), [basketball terrace](screenshots/ue5-park-terrain-basketball.png). Desktop main 0.15.1 does not yet contain these terrain changes. The terrain branch extends the cumulative sports integration; existing prototypes, the original mixed checkout, saves, Unity and packaged releases remain intact.
