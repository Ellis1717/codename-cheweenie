# Codename .... Cheweenie — Marty coats and jackets

Open **Pause → Controls → Clothes & costumes → Marty jacket**. Left/right or D-pad cycles backward/forward through None and four jackets. Enter / A-Cross cycles forward; the mouse row and minus/plus buttons also work. The same wardrobe opens from Backpack → Clothes on / off and returns to the backpack. Resume to inspect the outfit. The wardrobe heading is now **Costumes & Marty jackets**.

| Saved ID | Original wardrobe option | Visual details |
|---|---|---|
| 0 | None | Marty wears his regular fur and collar. |
| 1 | Rainy-Day Yellow | Yellow long-back shell, pale trim and a dark belly fastening. |
| 2 | Cozy Red Fleece | Red body, raised dark ribbed collar and warm hem trim. |
| 3 | Navy Winter | Navy padded silhouette, raised collar and dark seams. |
| 4 | Pink Quilted | Pink padded body and contrasting dark collar/seams. |

Each jacket is optional and saves with solo progress. Selecting a jacket removes Marty's hot-dog costume; enabling the hot dog removes his jacket. Lakitu and Rio retain their independent costume switches. Existing saves without a jacket field default to None. Invalid saved jacket IDs fall back to None.

The clothing uses original runtime-generated curved shells, seam geometry and existing project tint materials. Sleeve-free cuts expose the legs, face and tail. The attachment follows the retained spine reference frame across fur-detail changes and pet poses. It has no collision, overlap events, navigation influence or stat bonuses. These are cosmetic prototypes with rigid spine attachment, not simulated cloth. The existing LAN costumes remain available; the new jacket choice is currently solo only.

Amazon product photographs informed the generic cut and color choices. Geometry and materials are original; the photographs remain external design references, with no seller logos or photos added to Content. References inspected November 3, 2026:

- [Amazon dachshund raincoat listing](https://www.amazon.com/dp/B081SGNNNH).
- [Yellow long-back raincoat reference photo](https://m.media-amazon.com/images/S/aplus-media-library-service-media/5addc842-f3c2-4320-8403-8adaa62c52de.__CR0%2C0%2C970%2C600_PT0_SX970_V1___.jpg).
- [Red high-collar coat reference photo](https://m.media-amazon.com/images/I/618XcLVoeqL._AC_SL1000_.jpg).
- [Navy padded coat reference photo](https://m.media-amazon.com/images/I/61xJmi1oQGL._AC_SL1500_.jpg).
- [Pink two-tone quilted coat reference photo](https://m.media-amazon.com/images/I/61pQTf94cEL._AC_SL1500_.jpg).

Run `BUILD.ps1`, then `TEST_JACKETS.ps1 -Render`. Acceptance uses the real map and a separate `CheweenieJacketAcceptance` profile, with synthetic keyboard/gamepad/mouse input and camera/action fixtures. It checks outfit switching, winding, materials, collision, all three fur models, lying/rolling, jumping, traversal, hot-dog exclusivity and another-process persistence. `TEST_COSTUMES.ps1` retains the original three-pet coverage and `TEST.ps1` checks traversal. The test runner uses the engine-supported `UE_SKIP_UBT_SDK_SETUP=1` only in game processes after native compilation, avoiding a redundant startup SDK probe; it does not change compiler validation or engine files. Actual completed results belong in README and the evidence report. Physical hardware and standalone packaging must be recorded separately.

Actual rendered prototypes: [Yellow](screenshots/ue5-jacket-yellow.png), [Fleece](screenshots/ue5-jacket-fleece.png), [Navy](screenshots/ue5-jacket-navy.png), [Pink](screenshots/ue5-jacket-pink.png). [Low fur](screenshots/ue5-jacket-pinklow.png), [lying](screenshots/ue5-jacket-lying.png), [rolling](screenshots/ue5-jacket-rolling.png). The stiff collar and saddle-like side silhouette remain prototype geometry; cloth draping and deformation are unfinished.

Validation: native Editor compilation and 86 real-map assertions passed (21 jacket + 3 restart, 44 costume/backpack + 2 restart, 16 movement). Input was synthetic. See [actual results and repaired validation findings](evidence/ue5-jackets-build.json). Standalone packaging and physical controllers remain unverified.
