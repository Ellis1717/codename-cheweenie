# Codename .... Cheweenie — screen overlays

Open **Pause → Controls → Screen overlays**, or **Game & versions → Graphics, overlays & controls → Screen overlays**. The shared menu also works in LAN and the Ratchet memory room.

Choose a row with arrows / D-pad. Enter / A-Cross, left/right, or the mouse row and minus/plus buttons toggle visibility. The list scrolls as selection moves. Back / B-Circle returns to graphics. **Restore overlay defaults** enables every overlay. **All gameplay overlays** is a master switch: switching it back on restores each individual choice. Pause and settings menus remain available with the HUD hidden.

Independent switches cover objectives, control hints, pet status/activity, health, energy, backpack summary, interaction prompts, projected objective labels, local map, compass, current location, destination guidance, Halloween paw-pass, direct trick guide, messages, room status, other pets' names, home upgrades/bonuses, and cat reaction labels.

These are visibility preferences. Hiding health, energy, inventory or prompts does not disable their game mechanics. Existing Controls feature switches still govern whether a feature exists; both its feature switch and overlay switch must allow it to appear. The direct trick guide has its own visibility choice, independent of ordinary control hints. The full map opened explicitly with M / D-pad Up remains available when the local map or master overlay is hidden.

Preferences apply immediately, survive map travel and restarts, and remain local to each PC in `Saved/OverlayPreferences.json`. Missing fields default to visible; unknown fields are ignored. No checkpoint schema changes are needed. Archived executables retain their original menus and behavior.

Run `BUILD.ps1`, then `TEST_OVERLAYS.ps1` (optionally `-Render`). Acceptance uses a separate `OverlayAcceptance.json` and `CheweenieOverlayAcceptance` checkpoint. It exercises independent switches, the master switch, keyboard and synthetic gamepad menu input, restore defaults, disk reload, a second process, solo-to-memory-room travel, and actual cat reaction visibility without losing rewards. Rendered checks capture the default HUD, hidden HUD and settings menu. Physical controller checks must be recorded separately. See README for actual completed validation.
