# Marty: energy and cozy actions (0.6)

The Unreal demo now adds pet needs and a small sandbox alongside the existing patrol. Start/continue the game; the new HUD shows energy, mood and the carried item. Existing saves gain compatible defaults. The 0.5 package, original character resources and Unity game remain available.

## Controls

| Action | Keyboard | Xbox / PlayStation |
|---|---|---|
| Jump from the ground | Space | Y / Triangle |
| Bark (0.6.1) | B | LB / L1 |
| Dig at a marked patch | F | RT / R2 |
| Pick up, bury, eat, hop onto couch, watch/stop TV | E near the relevant object | X / Square |
| Drop the carried toy | Q | LT / L2 |
| Hurry while sufficiently rested | Shift | A / Cross |
| Recover home, keeping carried/buried items | H or pause menu | Pause → Return Marty home |

Patrol interactions keep priority near the active cat/fort destination. Otherwise nearby toys and cozy objects use the same Interact button. Controls are also listed in the in-game help. Physical controllers still need owner verification; automated tests send the same generic gamepad keys.

Barking, action effects and location music are described in [the audio guide](UNREAL_AUDIO.md).

## Energy and moods

Energy starts at 80/100. Walking drains gently; hurry, digging and jumping cost more. Below 25 Marty becomes Tired, walks more slowly and cannot hurry. He can always walk and return home. Idle rest replenishes energy; watching TV restores it faster, and kitchen food gives a larger refill. Playing with toys, burying treasure, eating and jumping briefly make a sufficiently rested Marty Excited, with a livelier tail. Otherwise he is Normal. Tired takes priority over excitement. This is a gentle pet game, without health damage or death.

Default tuning in DogLife.h/.cpp: walk 0.04 energy/second, hurry 0.65/second, idle recovery 0.65/second, TV recovery 1.5/second, six per dig, eight per jump and 30 per meal. The bowl refills after 25 seconds of active play. Normal walk/hurry are 100/230 cm/s; tired walking is 70. These are game-balance assumptions, not real animal-care guidance. Editable component properties expose the main rates for future Blueprint/C++ tuning.

## Dig, carry, bury and recover

The tennis ball starts on the headquarters floor. A treasure pebble starts in the front garden. Approach and press Interact to carry one in Marty's mouth. The object follows the animated head; Drop places it back on a nearby surface.

There are three marked soil patches: one in the front garden and two at the park. Three timed digs open a hole. Bring a carried item to an open patch and press Interact to bury it. One item fits in each hole. Dig again to recover it, then pick it up or bury it elsewhere. Paws scratch alternately during the digging action. These are shallow visual dig spots over retained collision ground, not arbitrary landscape excavation, so Marty cannot fall into a pit.

Hole depths, buried contents and the carried-item ID save automatically. Loose dropped toys stay where placed during the session and return to their safe starting locations on reload. The save clamps energy and array bounds and prevents an item from being both carried and buried, or two items occupying one hole. A new indoor patrol retains pet needs and toys. Home/door recovery cancels transient actions and keeps possessions. There are no purchases or consumable inventory losses.

## Couch, TV and kitchen

Jump with Space/Y-Triangle while moving, or approach the front of the sofa and use Interact for an assisted hop. The hop uses vertical launch followed by forward movement and lands on the real sofa collider; it does not teleport Marty onto a fake seat. Airborne legs tuck rather than planting paws on the floor below.

On the sofa, Interact starts TV rest. A fixed cozy camera shows Marty and the TV, and his knees flex into a resting posture. The screen shows an original, silent animated ball scene. Interact again or walk away to stop watching and return to the normal camera. The TV stays on when Marty leaves; its on-state is saved. No commercial video, stream or third-party artwork is included.

The kitchen corner has a mint counter/fridge and a food bowl. Approach the bowl and Interact. A short kitchen camera view shows Marty lowering his head for a meal. He gains energy and becomes excited if sufficiently rested. A cooldown prevents instantly repeating meals. Pet energy, action timers, food cooldown and TV animation stop while menus are open; the underlying world retains the existing soft-pause policy.

## Extension and verification

DogLife is a small pawn component that owns needs, contextual actions, toy/hole state and original TV props. CozyAdventure still owns the mission/save slot; Neighbourhood owns outdoor layout; CharacterMovement owns collisions/jumps; MartyGait owns the skin pose. The changes do not replace the retained model or the original patrol. Save additions use named fields and compatible defaults rather than resetting existing progress.

Run BUILD.ps1, then TEST_LIFE.ps1. Add -Render for actual captures or -GameExe to run the standalone build. The test uses only CheweenieLifeAcceptance, verifies action input, real sofa landing, meals, digging, object states, mood, pause and a second-process reload, then deletes only that acceptance slot. Position and energy fixtures set up the tests; this does not claim every route was manually walked. Re-run existing movement, gait, exterior and patrol checks before finalizing a new package. Reports and captures belong under Saved/Packaged06; FINALIZE_PACKAGE.ps1 requires the new life reports as well as the existing checks before the desktop launcher selects 0.6.
