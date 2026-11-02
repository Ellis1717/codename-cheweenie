# Home skills and progression

Codename .... Cheweenie 0.13.0 adds a solo headquarters loop around the existing pet actions. The garage/gym is an inferred prototype annex, not a measured reconstruction of the owner's home. The house, streets, existing saves, Unity fallback and older packages are retained.

## Try it

Start **Play Cheweenie**, then continue the adventure. Use **E / Square-X** at a nearby station; its prompt shows the action and price. The **M / D-pad Up** map includes the bed, garage door, workshop and three training stations. Cross-room destinations guide you through the appropriate door. **H** still returns Marty safely home and cancels training.

| Place | Interaction and effect |
| --- | --- |
| Marty's bed, in the cozy room | Lie down to restore energy and health. After three seconds, gain Rested for three minutes. Movement or interact gets up. |
| Kitchen food bowl | The existing eating action restores health and energy; home progression adds 10 energy and a two-minute fuel bonus. |
| Garage + gym door, near the room's upper-right corner | Enter a separate walkable annex; the marked return door leads back to headquarters. |
| Garage harness bench | Spend praise stars on a visible padded Snuggle Shield harness. Each tier reduces incoming damage by 15%, up to 45%. |
| Garage foam tester | Optional, clearly labelled ten-point virtual damage test. Armor reduces the damage. Six-second cooldown. Rest or eat afterward. |
| Gym strength mat | Start a session, then finish a shake-paw nearby. Each tier makes digging quicker and cheaper in energy. |
| Gym agility mat | Start a session, then finish a roll-over nearby. Each tier raises jump velocity by 6%. |
| Gym speed lane | Start a session, then hurry through the gold finish. Each tier increases walk/jog/run speed by 5%. |

## Skills receive responses

Perform a trick within 1.8 game metres of Lakitu or Rio to receive a named original joke, happy cat motion, a response sound and a brief approval label. Lie-down, completed roll, completed shake and bark each count. The gym has a Coach Keetz practice response, allowing solo practice without bringing a cat into the annex; it is a coaching callout rather than another cat actor.

Hold **L1 / LB** and press **Cross / A** to lie/stand, **Circle / B** to roll, **Square / X** to shake or **Triangle / Y** to bark. Keyboard: **Left Ctrl + 1 / 2 / 3 / 4**. The existing trick menu remains available. Standing up is not another rewarded trick.

Each witness rewards different tricks with one praise star apiece. Completing all four earns two bonus stars and starts a fresh variety round. A saved eight-second shared cooldown limits repeated rewards; repeating the same trick within a round gives a response but no star. Start with three stars; carry up to 99. Reactions respect companion, world-label and sound-effect switches. The cats' current whole-body happy motion remains stylized; this update adds no new feline skeletal animation.

## Editable balance and persistence

All upgrades have three tiers. Gym sessions cost 2, 4, then 6 stars; harness upgrades cost 3, 5, then 7. Training requires 30 energy to start and spends eight on successful completion. A session lasts up to 30 playing seconds; timing out or leaving spends no stars. The reward check revalidates funds before purchase.

Strength changes dig duration from 0.9 seconds by -0.12 per tier and dig cost from six energy by -0.75 per tier. Jump velocity starts at 440 cm/s. Speed uses the existing walk/jog/run speeds; tired walking remains 70 cm/s. These are game balance choices, not measurements of pet biomechanics.

The bed adds four energy and 1.5 health per second to normal lying recovery, respecting the separate health/energy switches. Fuel reduces movement energy drain by 25%; Rested reduces it by 15%. Together they leave 63.75% of the ordinary drain. Bonuses refresh their duration rather than stacking repeatedly. Pausing/title screens freeze timers; saved remaining durations resume after reloading. Trick rewards, stars and tiers persist in the existing compatible adventure save. Transient resting/training never resume as stuck actions.

**Controls → Pet feature switches → Home upgrades & reactions** disables rewards, bonus effects and visible harness while retaining purchases. Doors remain usable. Food, health, energy, tricks and hurry retain their existing independent switches. The harness hides beneath Marty's hot-dog outfit to avoid overlapping costumes; armor protection still applies. This progression is currently solo; the retained LAN room does not share stations, upgrades or currency.

Implementation: `HomeProgression.cpp/.h`, the appended save fields in `CozyAdventure.h`, and hooks in `DogLife.cpp`/`DogTricks.cpp`. `TEST_HOME.ps1` launches the real map twice with an isolated save slot and optional legacy-save input; it uses synthetic keyboard/controller events and explicit position, currency and cooldown fixtures. Final build evidence is recorded separately after validation.

## Actual validation

The final native Editor/Game builds, full cook/archive and subsequent native restaging passed. The standalone gate passed 605 assertions, including 39 new home-loop/reload checks. Final Editor home checks passed 39 and movement passed 16. Four private-map, 18 real archive-return/previous-core movement and 22 additional Ratchet seed checks bring the selected final runs to 704 assertions. Three recorded bark/music/mute waveform checks passed. Input was synthetic; LAN used two processes on this machine. Physical controllers, separate PCs, foreground interaction and broad performance remain unverified.

Four new actual gameplay captures were reviewed: [cat reaction](screenshots/ue5-home-cat-reaction.png), [bed](screenshots/ue5-home-bed-rest.png), [harness workshop](screenshots/ue5-home-armor-workshop.png), [gym finish](screenshots/ue5-home-gym.png). The bed's follow view is close to the room boundary. The annex uses prototype primitive furnishings. An opaque portal marker originally blocked the garage camera; the released version uses open frames.

All 43 current payload hashes and 2,791 files across 35 historical archives passed integrity checks. The actual Windows PowerShell 5 round trip from 0.13.0 to 0.12.1 and back passed and restored the full archive index. Checkpoint/private resources and previous shortcut were retained. Separate UI source edits in the original checkout were preserved rather than overwritten or combined into an untested Editor module. See [the evidence](evidence/ue5-home-progression-build.json) for test scope, fixtures and initial failures that were repaired.
