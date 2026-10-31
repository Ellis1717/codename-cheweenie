# Marty: detailed coat, needs, tricks and body controls — 0.7

This update combines the owner's requests for more visible texture, health/inventory displays, dog tricks, three paces and an optional two-stick body-control mode. Existing releases and source assets remain available.

## Play and controls

| Action | Keyboard | Xbox / PlayStation |
|---|---|---|
| Open tricks, pace and control choices | C | B / Circle |
| Open backpack | Tab | View / Touchpad |
| Choose / confirm / back | Arrows / Enter / Backspace | D-pad / A-Cross / B-Circle |
| Bark directly | B | LB / L1 |
| Get up from lying | E or move | X-Square or left stick |
| Move / temporary hurry | WASD / hold Shift | Left stick / hold A-Cross |
| Jump / dig / drop | Space / F / Q | Y-Triangle / RT-R2 / LT-L2 |
| Camera preset / return home | V / H | R3 / pause → Return Marty home |

The Tricks menu offers lie down/get up, roll over, shake a paw, bark, a walk/jog/run cycle, and the standard/body-sway control switch. Standard controls remain the default. The current control mode is described in the gameplay HUD. Right-stick camera control is restored when switching back.

Lie/roll/shake briefly use an obstruction-tested side camera so the action is visible. Getting up or finishing a timed trick returns to the normal follow view.

In body-sway mode, the left stick steers movement relative to Marty. The right stick produces a bounded body lean and damped turning; the camera follows his heading. A mass/spring/damper calculation settles the lean after release. Limits are 12 degrees of roll and eight of pitch, reduced when tired. Foot IK compensates for the shifted hips. Menus freeze the state; timed tricks and TV rest ignore sway input. This combines procedural poses with Unreal CharacterMovement collision, gravity and acceleration; it is not a full ragdoll or biomechanical animal simulation.

## Health, energy and the backpack

Health and energy each have a visible bar. Health starts at 100, accepts Unreal damage events for future gameplay, and recovers through rest and food. There is no new combat or damage hazard in this cozy demo. At zero, Marty safely returns home with 25 health rather than dying. Energy retains tired/normal/excited states; lying down restores it faster than standing idle. Food restores 30 energy and 20 health. The defaults are editable game-balance assumptions.

The backpack has four slots; the demo currently provides two carryable toys. Pick up a toy in Marty's mouth, open the backpack, and choose Stow. Retrieve returns one stored toy to his mouth for dropping or burial. The HUD shows bag occupancy; the backpack page lists its contents and the mouth item. An item cannot be simultaneously carried, stored or buried. Bag IDs, health, energy, selected pace and control mode save; active trick poses and spring offsets reset on reload.

Walk/jog/run request 100/155/230 cm/s, with distinct gait timing. Acceleration and braking act on the character motor; analog stick strength still modulates speed. Shift/A-Cross temporarily requests run. Tired walking remains available at 70 cm/s. Jog costs less energy than run. These are tuned game speeds, not measured animal biomechanics.

## Visible coat detail and retained sources

The new variant retains 4096×4096 grain/normal maps, 16,000 short skin-weighted crossed-ribbon hairs, muzzle whiskers, a checker collar, and a blue tag without personal details. Tan/darker-ear/cream-chest colouring and face proportions follow the retained private photos and start-page cover. The 169,958-vertex mesh retains the existing 24-bone anatomy. It is original inferred geometry and an approximation of Marty, not a scan or a claim of photographic identity.

Fine fur follows the animated skin and uses a hair shading material. It is polygonal fur, not an Unreal strand Groom or a separate fur simulation. [Epic's Groom documentation](https://dev.epicgames.com/documentation/unreal-engine/hair-rendering-and-simulation-in-unreal-engine) describes that separate strand-based workflow. The source maps and geometry are generated from project code; no private backgrounds or downloaded samples are embedded. Original models, Blender source, textures and independent previews remain untouched.

SourceArt/MartyRealism and Scripts/build_marty_realcoat.py retain the new variant. IMPORT_REALCOAT.ps1 imports only its separate Content/Adventure/MartyRealism folder. The material importer checks graph connections and preserves existing expressions while updating the generated palette. The palette is retained in UV channels with the FBX V convention explicitly restored. Use -CheweenieSmoothMarty for the retained smooth skinned model, or the preserved 0.6.1 package. The older static fallback remains available with -CheweenieLegacyMarty.

## Verify and extend

The final 0.7 build passed **151 native assertions**: 131 standalone, four private-map and sixteen legacy editor-game movement checks, plus three audio waveform checks and a full-HD coat colour/resolution check. Native Editor/Game compilation, full cook/archive and final staging succeeded. Rendered DX12 play and headless reload phases were used; final logs contained no Error/Fatal categories or shader fallbacks. The actual portrait, resting/roll/shake, backpack and body-control captures were inspected. [Build summary](evidence/ue5-07-build.json), [two-stick/trick assertions](evidence/ue5-07-tricks.json), [reload assertions](evidence/ue5-07-tricks-resume.json), [actual portrait](screenshots/ue5-07-portrait.png).

DogLife owns needs/inventory, DogTricks owns the new actions, DogBodyControls owns the spring mode, and MartyGait owns posing/foot IK. Save fields use compatible defaults. TEST_TRICKS.ps1 exercises real menu bindings, skinned poses, damage/recovery, acceleration, both joysticks together, settling, mode restoration and a second-process reload in its isolated acceptance slot.

TEST_GAIT.ps1 -Render -Width 1920 -Height 1080 captures the actual model. CHECK_COAT.ps1 checks image dimensions and tan torso pixels at a projected body point. FINALIZE_PACKAGE.ps1 also rejects shader fallbacks and requires fresh trick, persistence, audio and existing gameplay reports. Physical controllers, subjective likeness/listening and measured performance remain separate owner checks.

An initial dense-fur import exhausted the machine's memory; geometry density was reduced while retaining the 4K maps. Render review found incorrect material connections and a UV convention mismatch; the importer now asserts its connections and the release gate checks actual tan output. Synthetic confirm presses in one frame were also separated in the inventory acceptance fixture. Those early attempts are not passing release evidence.
