# Codename .... Cheweenie version 1 release candidate roadmap

Planning baseline: **2026-11-06**, desktop main **0.15.1**, Unreal Engine **5.8.4**. A version 1 release candidate is a complete, polished build that could become version 1 unchanged if final testing finds no blockers. More features and a higher automated test count alone do not establish readiness.

The recommended route is to finish the existing neighbourhood adventure in stages: qualify the current game, finish the crew, prove one complete outing, extend co-op, finish the campaign, then freeze and qualify the release. These are proposed gates; none is marked complete by this roadmap.

## Build one connected game

Owner direction confirmed **2026-11-06**: continue combining standalone features into the full game. Develop and test features in focused pieces, then integrate compatible work into the cumulative Unreal adventure. Players should reach current activities through the world or its menus, using the same controls, settings and compatible progress. Retain standalone prototypes and archived releases for development and historical fallback.

For example, an outing can start at home, include a neighbour favour, lead to soccer at the park, and finish with Rainbow Guy and a cozy rest at home. Each activity should have a clear entry, feedback and return path. Optional activities can remain independent of campaign completion; persistent rewards and temporary session scores must be clearly distinguished.

Each integration milestone must:

- [ ] Identify the source feature, its dependencies and what is already in the desktop release.
- [ ] Connect its entry and return paths to the adventure; reuse shared controls, settings and save conventions.
- [ ] Check transitions with existing activities, including pause, carrying, companions and return-home recovery where applicable.
- [ ] Compile and test the combined game, review actual renders and preserve existing progress.
- [ ] Publish an updated cumulative package only after combined validation; record which improvements are still development-only.

Integration is recurring work throughout the roadmap. Maintain the feature inventory so useful standalone behavior is carried forward deliberately, including any retained Unity behavior that still needs a native implementation.

## Current foundation and remaining work

The cumulative game already includes home progression, neighbourhood exploration, Marty movement and cozy actions, companion follow/stay, clothes, digging, Rainbow Guy, optional seasonal activities, indoor LAN and the soccer/baseball/basketball practice park. The 0.15.1 release record reports 1,081 required packaged checks and 1,281 selected packaged/Editor assertion executions. These establish useful regression coverage, not final art approval or human play quality. [Release evidence](evidence/ue5-park-main-build.json).

The major remaining work is articulated cats and accepted pet likeness, a complete connected campaign, outdoor co-op, polished activity feedback, onboarding, measured performance and physical-device/second-PC testing. Existing indoor LAN does not establish outdoor co-op; existing sports practice does not establish team sports.

## Ordered milestones

| Order | Deliverable | Complete when |
|---|---|---|
| 1 | Agree version 1 scope and qualify the baseline | One scope checklist, named test hardware, reproducible cumulative build, preserved saves and a measured traversal baseline exist. |
| 2 | Finish Marty, Lakitu and Rio | All three have accepted in-game appearance and grounded movement; cat rigs and interactions are finished. |
| 3 | Finish one complete neighbourhood outing | A new player can leave home, discover and finish three favours, enjoy the park, return home and resume progress without help. |
| 4 | Prove outdoor two-player play | All three core pet pairings complete the shared slice on two PCs, including reconnect and save/reload. |
| 5 | Finish the agreed campaign and world | All promised locations, missions and favours have complete play, presentation, persistence and an ending. |
| 6 | Reach beta and freeze features | Content is complete; usability, performance, art/audio and save resilience meet the agreed targets. |
| 7 | Produce and qualify 1.0.0-rc.1 | The exact packaged candidate passes release checks, independent playthroughs and distribution/privacy checks. |

Work through these gates in order. Smaller releases within a gate are encouraged; a gate stays open until its exit evidence exists. This preserves U1–U6 priorities and follows the existing OW-00–OW-05 plan. [Original handoff](UNREAL_HANDOFF.md), [open-world design](OPEN_WORLD_DESIGN.md), [measurable targets](OPEN_WORLD_TARGETS.json).

## 1 Scope and baseline

- [ ] Use the existing proposed first-release envelope as the starting scope: five districts, six campaign missions, 14 optional favours and at least five activity patterns. Review each promise before production; counts measure scope, not quality.
- [ ] Keep solo and two-player LAN with Marty, Lakitu and Rio as the proposed completion modes. Explicitly decide whether outdoor co-op is required for version 1. This roadmap includes it; a smaller solo version needs an explicit scope revision.
- [ ] Define the sports promise as enjoyable soccer, tee-baseball and basketball practice. Team AI, full baseball rules and competitive multiplayer require a separate scope decision.
- [ ] Inventory every promised feature as packaged, development-only, unfinished or deferred. Reconcile source branches and focused PRs into one cumulative main; preserve the original mixed checkout.
- [ ] Record CPU, GPU, RAM, Windows version, resolution, graphics settings and controller models. Capture repeatable indoor, outdoor and park frame-time routes.
- [ ] Reproduce the packaged baseline, verify checkpoint/settings/private resources and demonstrate a retained rollback.

Keep vehicles, combat, a larger city, procedural missions, three-player expansion and internet matchmaking after version 1 unless the scope is deliberately changed. Do not assign calendar dates until baseline and character work reveal the actual pace.

## 2 Finish the playable crew

- [ ] Finish Lakitu and Rio skeletal articulation, paws, tail, idle, walk/trot, turns, stopping, jumping and rest/interaction motion; preserve their reference markings.
- [ ] Review Marty, Lakitu and Rio against retained owner references using actual front/side/three-quarter gameplay views and clips. Include Marty ears, coat, eyes, collar and stance transitions.
- [ ] Tune ground contacts on five representative surfaces. Use the existing proposed targets of under 2 cm obvious paw penetration and under 3 cm planted-foot drift over one second, alongside visual review.
- [ ] Check carrying, cozy interactions, clothes, cameras, follow/stay and recovery with the finished bodies. Keep Ratchet and other supported optional content functional and accurately described.

**Exit:** owner visual review and movement regressions pass in the real game. Automated geometry checks cannot substitute for likeness and animation approval. This closes the remaining U2/U3 work before wider environment expansion.

## 3 Finish one complete outing

- [ ] Connect home, yard and park approach with two useful route loops, six discoveries and three complete favours, following OW-02.
- [ ] Teach movement, camera, interaction, carrying and recovery through play. Include Cheween in Between and Cheeween Cozy in the experience.
- [ ] Finish park legibility: signs, interact prompts, scoring, reset feedback, baseball base order and clear return routes. Review ball response, camera framing and difficulty with human players.
- [ ] Save completed favours and promised rewards; keep temporary sports scores explicitly session-only unless scope changes.
- [ ] Test stairs, doors, props, boundaries and return-home recovery with guidance both on and off.

**Exit:** at least four of five fresh-start sessions reach play within three deliberate menu selections and a satisfying interaction within two minutes. Target the first favour within five minutes. Players complete the outing without developer intervention, then reload into the correct progress.

## 4 Prove outdoor co-op

- [ ] Extend one shared object, one favour and one district transition before expanding the full campaign.
- [ ] Complete the slice as Marty + Lakitu, Marty + Rio and Lakitu + Rio; never require an absent third pet.
- [ ] Keep shared objects/checkpoints authoritative on the host and guest solo progress independent.
- [ ] Test contested pickup/drop, disconnect while carrying, reconnect, travel readiness and host save/reload. Run 100 contested item trials with zero duplicated or lost items.
- [ ] Test two physical PCs with host/guest role swaps, then the proposed 80 ms round-trip delay and 1% packet loss conditions. Local two-process tests remain separate evidence.

**Exit:** every supported pairing finishes the shared slice and recovers without lost items, progress or guest data. Sports can remain solo practice within the cumulative game; do not imply replicated sports without implementation and testing.

## 5 Complete the campaign and neighbourhood

- [ ] Build the agreed five districts, six missions and 14 optional favours from the proven slice; revise the scope explicitly if those numbers change.
- [ ] Give each mission a readable start, objective, payoff, completion state and save/reload path. Provide a clear campaign ending and continued free play.
- [ ] Finish environment collision, pet-height landmarks, lighting, school-side/neighbour routes and interaction placement. Preserve the owner's names and confirmed pet details without inventing markings or breeds.
- [ ] Build terrain with layers, gentle hills, varied elevations and walkable transitions. Give ball fields believable crowns, mounds and drainage slopes; keep courts level where play requires it. Fit markings, props and ball motion to the surface, and verify uphill/downhill keyboard and controller traversal.
- [ ] Add original character comedy, captions and rewards without blocking interaction. Mission understanding must survive dialogue mute.
- [ ] Complete each critical path in solo and supported co-op, including out-of-order exploration, return-home recovery and interrupted sessions.

**Exit:** the content inventory has no missing mandatory step, placeholder critical interaction or unreachable reward. Expand locations only after the current routes provide interesting play.

## 6 Beta polish and feature freeze

- [ ] Freeze new features. Track crashes/data loss as blockers, progression failures as critical, and separate cosmetic issues from both.
- [ ] Finish readable menus, controller hints, camera behavior, captions/text sizing, audio balance and mute controls. Review actual rendered screens and normal gameplay views.
- [ ] Complete real keyboard/mouse, Xbox and PlayStation walkthroughs, including menus, focus loss, controller unplug/reconnect, persistence and recovery. Physical signal detection alone does not pass gameplay.
- [ ] Qualify the proposed 1080p Medium/60 FPS profile and 720p/30 fallback on named hardware. Measure warmed ten-minute indoor/outdoor/co-op routes. For the 60 FPS profile, target p95 frame time ≤20 ms, p99 ≤33.3 ms and no gameplay hitch over 100 ms; record loading and shader warmup separately.
- [ ] Preserve progress across three save/reload cycles and a forced-shutdown recovery trial. Test supported older-save upgrades, missing/corrupt-save recovery and backups.
- [ ] Run at least two complete human campaign playthroughs, including one fresh profile and one upgraded profile. Capture defects, fixes and focused retests.

**Exit:** no open crash, data-loss, progression or required-control blocker; scope is complete and the declared hardware profiles pass. Any remaining minor issues are documented and accepted for the candidate.

## 7 Version 1 release candidate

- [ ] Tag the exact source/assets and package **1.0.0-rc.1** from the cumulative main. Record hashes, supported Windows/hardware profiles, controls, known issues and save compatibility.
- [ ] Run the full current acceptance suite plus new campaign, co-op and resilience tests against that exact package. Recheck rendered gameplay and runtime logs.
- [ ] Launch the distributable on a second Windows PC without the editor or development paths. Verify prerequisites, first start, continue, audio, controls and exit.
- [ ] Inspect the transfer package for private addresses/maps, local dog rosters, owner saves, logs, secrets and unlicensed assets. Keep personalized local data outside the public distribution.
- [ ] Verify the launcher, update/rollback instructions and previous-release retention. Keep Unity and the comparison preview intact.
- [ ] Perform a final human playthrough of the exact candidate and owner review of the remaining issue list.

**Decision:** a blocking defect requires a fix and a new candidate, with affected checks repeated and complete release gates rerun. Promote to **1.0.0** only after candidate acceptance. A roadmap or passing automated count never promotes a build by itself.

## Next work session

Start with milestone 1: reconcile the cumulative source and release evidence, create the feature inventory, record hardware, capture the baseline route and open the first defect list. Then finish cat articulation under milestone 2. Owner participation is needed for scope choices, pet likeness, physical controller play and final enjoyment review; implementation and repeatable automated checks can proceed between those sessions.
