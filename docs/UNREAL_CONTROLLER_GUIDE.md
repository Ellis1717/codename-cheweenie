# Codename .... Cheweenie — controller-aware controls

The native Unreal development game has a device-aware **Controls & cameras** screen with an original, code-drawn controller illustration and a matching button legend. Xbox uses A/B/X/Y, LB/RB and LT/RT; PlayStation uses Cross/Circle/Square/Triangle, L1/R1 and L2/R2. Keyboard input shows a WASD/mouse illustration and keyboard legend. Gameplay hints rendered through the shared HUD helper use the selected button names.

**Controller display** is the final entry in Controls & cameras. Use keyboard arrows, D-pad or the scrolling mouse menu to reach it; Enter, the bottom face button or a click cycles Auto → Xbox → PlayStation → Keyboard → Generic → Auto. The preference is saved locally across restarts and map travel, independently of adventure saves. It changes presentation only.

Auto follows real Slate key/button/mouse events and meaningful stick/trigger input, consulting Unreal's device registry for the event's device ID. Known Xbox/XInput and PlayStation/Sony/DualShock/DualSense identifiers choose the corresponding illustration. Unknown hardware uses neutral positional face-button labels. Small analog values below 0.25 are ignored. Devices can be swapped without restarting; the latest meaningful device input wins.

Windows drivers and translation software can expose a PlayStation controller as XInput, concealing its original identity. In that case Auto shows the Xbox identity reported to Unreal; choose PlayStation manually. This feature does not add a new controller driver or remap gameplay. Keyboard and existing supported gamepad controls are retained.

The independent content-only preview, archived Unity game and desktop standalone package are unchanged.

## Verification

Run `Unreal/UE5MovementLab/BUILD.ps1`, then `TEST_CONTROLLER_GUIDE.ps1 -Render` and `TEST.ps1 -Render`. The guide test runs the real map with simulated Unreal device descriptors and Slate keyboard/gamepad events, captures Xbox, PlayStation and keyboard screens, tests device swaps, unknown hardware, drift rejection and the keyboard-accessible manual preference, and restores the prior display preference. These are automated input checks, not physical-controller checks.

Native Editor compilation succeeded on both the accumulated development checkout and an isolated main-based review branch. All **14 rendered guide assertions** and **16 rendered movement assertions** passed on the real map using synthetic keyboard/gamepad input and simulated hardware descriptors. Three controls captures were visually inspected. Physical controller identification/handling remains unverified. The desktop package was not rebuilt.

Reports: [controller guide](evidence/ue5-controller-guide.json), [movement](evidence/ue5-controller-guide-movement.json). Captures: [Xbox](screenshots/ue5-controller-guide-xbox.png), [PlayStation](screenshots/ue5-controller-guide-playstation.png), [keyboard](screenshots/ue5-controller-guide-keyboard.png).

The isolated review branch additionally passed [14 rendered guide assertions](evidence/ue5-controller-guide-review.json) and [16 headless movement assertions](evidence/ue5-controller-guide-review-movement.json). Its three captures were also inspected. Two inherited include-order errors were corrected for clean native compilation. There were no failures in either final run.
