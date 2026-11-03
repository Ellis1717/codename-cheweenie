# Optional Halloween wardrobe

The clean PR #46–#49 integration reran all 44 wardrobe/backpack assertions and 2 outfit-reload assertions successfully with the shared overlay styles and recorded pet voices included. Fresh backpack and wardrobe captures were reviewed. [Combined results and separate known camera finding](evidence/ue5-pr46-49-integration.json).

Codename .... Cheweenie adds three independent cosmetic outfits to the active Unreal game:

- Marty: padded hot dog with bun sides, red saddle and mustard, based on the owner's costume photograph.
- Lakitu: pumpkin saddle with leaf/stem and a friendly face. This is an original proposed outfit, not a photographed accessory.
- Rio: yellow/black bumblebee jacket with pale wings, based on the owner's costume photograph.

**Hot-dog fit, 0.11.1:** the buns, sausage and mustard run from Marty's shoulders toward his rump. The attachment removes the imported spine bone's reference orientation before following the animated torso, rather than using bone axes as garment axes. The saddle is centered between shoulders and hips; the head, paws and tail remain exposed. The same component is used by solo and LAN Marty, and works with both detailed and smooth coat meshes. It remains a rigid padded overlay, without cloth simulation. [Actual side view](screenshots/ue5-hotdog-lengthwise-side.png), [top view](screenshots/ue5-hotdog-lengthwise-top.png), [0.11.1 results](evidence/ue5-hotdog-fit-build.json).

In the Unreal development project, open the backpack with **Tab / View** and select **Clothes on / off**. Choose Marty, Lakitu or Rio and press **Enter / A / Cross** to toggle their outfit. The menu stays open for multiple changes; **Back to backpack** or **Escape / B / Circle** returns to the clothing entry in the bag. All four backpack actions remain visible, including toy storage/retrieval and Close. Clothing choices use the same saved preferences as the existing wardrobe.

Backpack validation: native Editor compilation and 90 real-map assertions passed (44 wardrobe/backpack, 2 costume reload, 23 pet-life, 5 life reload, 16 movement), with two menu captures visually reviewed. Input was synthetic; physical controllers remain unverified. Initial default-cache play attempts stalled before assertions and were stopped; all retries passed with `-ddc=InstalledNoZenLocalFallback`. Reproduce the wardrobe suite with `BUILD.ps1` then `TEST_COSTUMES.ps1 -Render -NoZen`. The optional `-NoZen` switch selects that filesystem-cache fallback for both test phases. The desktop package and original checkout module are unchanged. [Actual results](evidence/ue5-backpack-clothes-build.json), [backpack menu](screenshots/ue5-backpack-clothes.png), [pet switches](screenshots/ue5-backpack-wardrobe.png).

All outfits default **off**. Open **Pause or Title → Controls & cameras → Halloween costumes (optional)**. Choose a pet with arrow keys/D-pad and press Enter/A-Cross to toggle; Escape/B-Circle returns to Controls. Each solo choice is saved separately; a new patrol preserves these preferences. Existing saves get the default off mask without a version reset.

The current 0.11.2 Bork or Treat patch also fixes Marty's underlying ear/tail skin joins and parent animation. The lengthwise outfit is retained and rechecked with both coats, lying/rolling and LAN peers. [Current side view](screenshots/ue5-marty-connected-hotdog-side.png), [attachment guide and actual results](UNREAL_MARTY_ATTACHMENTS.md).

In the separate LAN room, **T / D-pad Up** toggles your current pet's outfit. The choice replicates to the other player and follows pet selection, swapping to the matching costume. LAN outfit choices last for that connection and do not change solo saves.

These are original, padded costume overlays made from built-in engine shapes and eight original matte materials. Faces, paws and tails remain exposed. Marty's outfit follows his animated spine; cat outfits follow the retained static cat meshes. They have no collision, overlaps, navigation effect, stat bonuses, cloth simulation or physics straps. This does not upgrade the underlying pet models or claim photorealistic tailoring. Owner photo identities and likeness notes remain in the separate pet-reference PR.

## Rebuild and verification

Run `BUILD.ps1`, `IMPORT_HALLOWEEN.ps1`, then `TEST_COSTUMES.ps1 -Render`. The costume acceptance uses an isolated save slot, drives synthetic keyboard/gamepad menu events, checks independent choices/materials/collision/traversal and reloads preferences in a second process. `TEST_COOP.ps1` additionally checks network costume toggles and switching to Rio's outfit. Physical controllers and separate-PC LAN are distinct manual checks.

Final verification: Editor/Game builds and full cook/archive passed. The final standalone package passed 138 assertions: 20 costume/save, 53 two-process LAN, 34 companion and 31 patrol/save checks. Four 1280×720 packaged captures were visually inspected; final runtime logs had zero Error/Fatal/material fallback matches. All 43 immutable payload hashes passed the release gate. Physical controllers, two independent PCs and broad performance remain unverified. See [actual evidence](evidence/ue5-halloween-build.json). Existing World, LAN, solo, comparison and Unity builds remain available separately.

Launch **Play Cheweenie Halloween** on this PC’s desktop, or the separate distribution’s `PLAY_HALLOWEEN.cmd`. For LAN, use `PLAY_HALLOWEEN.cmd -Mode Host` and `PLAY_HALLOWEEN.cmd -Mode Join -Address <host-IPv4>` with the same build on each PC. These launchers preserve the earlier World/LAN launchers. The local default checkpoint and private map/labels were copied only when absent; their originals remain.

To package again, pass a new output folder to `PACKAGE.ps1 -OutputRoot`. Run the four packaged suites with the executable passed as `-GameExe`, placing outputs under a common test root in `Costumes`, `Coop`, `Companions` and `Adventure` subfolders; use `-Render` for `TEST_COSTUMES.ps1`. Then run `FINALIZE_HALLOWEEN.ps1 -PackageRoot <package-folder> -TestRoot <test-folder>`. It checks fresh reports/captures, runtime logs and payload hashes before installing the portable launcher. Inspect the actual captures separately. Raw family photos and private local data stay outside Git.
