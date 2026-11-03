# Codename .... Cheweenie — overlay style review

Fresh clean combined-checkout validation for PRs #46–#49 passed native Editor compilation, all 11 style assertions and all 4 second-process persistence checks using filesystem DDC fallback. This integration style run was headless; the earlier rendered comparisons below remain historical evidence. Backpack captures were freshly reviewed with the shared style code included. [Combined results, including the separate known rendered camera finding](evidence/ue5-pr46-49-integration.json).

Open **Game & versions → Graphics, overlays & controls → Overlay look**. The same selector is near the bottom of **Controls → Screen overlays**. Left/right or D-pad cycles backward/forward; Enter / A-Cross or the mouse row cycles forward. Mouse minus/plus buttons also work. The change is immediate, so resume play to compare the same scene.

| Preset | Look |
|---|---|
| Classic Teal | Existing teal panels and familiar colored text; the default. |
| Cozy Paper | Warm cream panels, brown ink, amber edges, and outlined text over scenery. |
| Midnight Pop | Deep violet panels, pink accents, pale text, and accent edges. |
| Clean Slate | Translucent charcoal panels, cyan accents, and quieter framing. |

The shared presentation covers solo and room HUDs, maps, control guides, prompts, notifications, and menus. Health/energy meters retain their semantic colors. Map lines and markers stay distinct on the light Paper background. Title illustrations and 3D scenery are retained.

Your chosen look saves on this PC alongside overlay visibility, survives room travel and restarts, and does not change which overlays are visible. **Restore overlay defaults** restores Classic Teal and enables all overlays. Existing preferences without a style field use Classic; invalid style values also fall back to Classic.

`TEST_OVERLAY_STYLES.ps1 -Render` captures the same gameplay view and graphics menu in all four presets, exercises keyboard, synthetic gamepad and mouse hitboxes, and checks wrapping, saving, room travel and a second process. It uses the isolated overlay acceptance profile. Physical controller interaction remains a separate check. Actual results and review captures are linked from README after verification.

Actual gameplay comparisons: [Classic Teal](screenshots/ue5-style-classic.png), [Cozy Paper](screenshots/ue5-style-paper.png), [Midnight Pop](screenshots/ue5-style-midnight.png), [Clean Slate](screenshots/ue5-style-slate.png). All use the same HUD layout and camera for review.
