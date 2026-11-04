# Upstairs at headquarters

The native Unreal solo adventure adds a second floor directly over the living room. A continuous staircase along the south wall leads to an upstairs landing, two bedrooms, a bathroom and a pet blanket nook. The layout is an original cozy prototype inferred from the existing footprint, not a surveyed representation of the owner's house.

Walk to the bottom of the stairs beside Marty's downstairs bed, then head east up the steps and turn left onto the landing. Walk with **WASD / left stick**; no interaction, jump or travel portal is required. **H** returns Marty safely to his downstairs spawn. The existing gamepad home/menu controls remain available.

Each bedroom has a bed, pillows, blanket, rug, nightstands, lamps and wardrobe. The bathroom has a vanity, sink, mirror, toilet and bath mat. Doors are open and collision matches the floors, walls and furniture. Furniture is scenery; the existing downstairs dog bed retains the recovery interaction. The upstairs pet nook can be used with the existing lie-down trick.

The 38 steps rise 10 cm each, below Marty's existing 12 cm step limit. The upper floor is at 380 cm. A split slab creates a real stair opening, with a handrail and landing guards. The retained `RoomShell` combines its ground-floor walls and ceiling into one mesh, so the solo runtime hides that visual batch and recreates its wall openings around the split ceiling. The original imported asset, downstairs floor and furnishings remain intact. The independent content-only preview and archived Unity game are unchanged.

The map appends stairs, bedroom, bathroom and pet-nook destinations without renumbering existing destinations. Guidance uses the staircase when changing floors, the local map filters markers by height, and the location readout identifies upstairs rooms. Indoor toy, food, bed, door, companion and patrol interactions check height to prevent reaching through floors.

Native Editor compilation passed. The final rendered upstairs run passed **18 assertions**, including full keyboard ascent/descent without jumps or traversal teleports, gamepad movement onto the landing, bedroom/bathroom access, wall collision, return-home recovery, stair guidance and protection against reaching downstairs interactions. Teleports set test starting positions only.

The existing movement (16), home progression/restart (34 + 5), toy life/restart (32 + 6), patrol/restart (25 + 6), navigation/restart (60 + 4), and companion/restart (30 + 4) suites passed **222 regression assertions**. Combined with the final upstairs suite, **240 selected assertions passed**. These regressions used identical production code before the final capture-only harness adjustment. All selected current runtime logs had zero Error/Fatal/material fallback/VSM overflow matches.

Four real-game captures were visually reviewed: [normal gameplay camera](screenshots/ue5-upstairs-gameplay.png), [bedroom](screenshots/ue5-upstairs-bedroom.png), [stairs](screenshots/ue5-upstairs-stairs.png), and [cutaway overview](screenshots/ue5-upstairs-overview.png). Only the overview hides the upstairs ceiling; normal gameplay and the other captures retain it. Furniture uses original engine primitives and the existing project material palette.

Run `Unreal/UE5MovementLab/BUILD.ps1`, then `Unreal/UE5MovementLab/TEST_UPSTAIRS.ps1 -Render`. See the [upstairs report](evidence/ue5-upstairs-smoke.json) and [regression results, source hashes and build scope](evidence/ue5-upstairs-build.json). The initial compile found an auto-pointer deduction error with Unreal's `TObjectPtr`; an explicit mesh pointer fixed it before the successful native builds.

Input was synthetic keyboard/gamepad. Physical controllers, cat stair-following, comparative performance and a new desktop packaged release remain unverified. The LAN room is unchanged. Existing unrelated checkout edits were retained; no PR was opened from this mixed checkout.
