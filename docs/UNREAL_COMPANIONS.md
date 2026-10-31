# Lakitu and Rio: companion behaviour — Unreal 0.9

Press **K / D-pad Down** during play for crew commands. Keyboard Down remains movement. The menu lets you independently ask Lakitu or Rio to follow/stay, call both to a clear nearby spot, send them home, or give either a friendly head bump. Pause also has a Cat companions entry. Close with B-Circle or Backspace. All actions work through the existing keyboard/controller menu system.

E / X-Square offers a nearby head bump when no mission, food/toy or doorway interaction takes priority. Each cat has an independent friendship score out of ten and a six-second greeting cooldown. Both reaching three earns the displayed Best Buddies badge and a Leo Ellis cameo. Friendship, follow/stay choices and the companion enable switch save; New Patrol preserves them. The badge is derived from friendship, rather than an extra inventory item.

Following cats use separate walking speeds (Lakitu 105 cm/s, Rio 90), acceleration/braking, smooth turns, and a 245 cm/s catch-up pace when far behind. Short sphere sweeps and floor traces limit wall clipping, steep drops and abrupt height changes. Cats stay nonblocking for Marty. They can slide along simple obstructions; this is not a NavMesh path planner. A cat that cannot proceed shows Waiting: move to clear floor and use Call both. Recall tests candidate floor spots and avoids blocking geometry. It does not blindly place a cat inside a wall.

Following cats accompany Marty through doors, walking-tour shortcuts and home recovery. A cat told to stay remains in that area for the current session. Call both or Send both home recovers separated cats. Reload returns cats to their retained home spots; following choices resume with Marty after starting play. Transient speed, reactions and outdoor positions are not saved.

Mission interactions, projected labels and map destinations track the cats' actual positions and indoor/outdoor areas. Authored mission Data Assets remain unchanged; stable stop IDs lakitu and rio resolve to live companions. Other stops retain their authored positions.

Companion mission visits allow a 150 cm interaction radius so following cats can be greeted at their normal spacing. A mission prompt takes priority over an optional greeting, and the HUD suppresses conflicting lower prompts. Ordinary head bumps use a closer range; Call both provides nearby greeting spots.

## Retained art and controls

The original Lakitu/Rio mesh assets, textures, colour markings and comparison previews remain intact. This milestone adds behaviour, whole-mesh turning and small idle/walking/reaction motion. **The cats are still static meshes, without articulated leg/tail animation or a fur rig.** Completing U3's skinned cat art and gait remains a next milestone; these checks do not claim realistic animal biomechanics or photographic likeness.

Pause → Controls & cameras → Pet feature switches includes Companion behaviour, the 25th independent feature switch. Disabling it restores the cats' home transforms without erasing friendship or follow choices. Reenabling resumes stored follow choices. The wide camera has less vertical offset so the crew fits, and route guidance sits above the main play area rather than across the pets.

## Routing optimisation

The street graph and connected-component data are compiled once per topology and reused across destination searches. Stationary players, unchanged destinations and paused menus skip periodic searches. Moving destinations still refresh when they move far enough. If a future system edits street geometry without changing its segment count, call InvalidateNetwork before refreshing the route.

The acceptance suite forces 64 repeated CPU route calculations, verifies one cached graph build, verifies idle/pause skips and explicit invalidation, and exports timing percentiles without coordinates. These timings describe routing work, **not whole-game FPS or GPU performance**. Private-layout runs remain headless and do not export screenshots.

## Extend and verify

CatCompanions owns locomotion, floor checks, safe placement, reactions and commands. Tuning is exposed on its native component. CozyAdventure owns saved friendship/choices; StopPosition/StopOutdoors adapt mission and map lookups without modifying authored assets. The new feature ID is appended so older saved switch bits retain their meaning. Existing saves use zero friendship and staying cats by default.

Run TEST_COMPANIONS.ps1 for headless gameplay and second-process reload checks. Add -Render for actual crew/menu/switch captures, -GameExe for the standalone build, or -MapFile with the retained local street file for headless private-map behaviour and route timing checks. The tests use synthetic keyboard/gamepad input and explicit position/progress fixtures. Physical controllers and subjective animation/audio quality remain owner checks.

Final 0.9 verification passed: 228 standalone assertions plus 34 private-map companion, six private-navigation, four private-map walking/recovery and sixteen legacy movement assertions (**288 native total**), three audio waveform checks and a full-HD coat check. Editor/Game builds and full packaging passed. Final crew/menu/switch captures were inspected; logs contained no Error/Fatal categories or material fallbacks. The private-map CPU routing samples measured roughly 0.033 ms median and 0.049 ms p95 across 64 searches with one graph build. [Actual results and repaired findings](evidence/ue5-09-build.json), [companion assertions](evidence/ue5-09-companions.json), [reload assertions](evidence/ue5-09-companions-resume.json), [private route timing](evidence/ue5-09-routing-performance-private.json), [actual crew](screenshots/ue5-09-companion-crew.png).
