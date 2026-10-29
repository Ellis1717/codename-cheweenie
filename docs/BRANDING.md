# Cover and front-end identity

The exact game title is **Codename .... Cheweenie**. The title screen renders the wordmark as live text over original cover concept art, with a small procedural paw mark. Warm ivory `#FAEDD1`, amber `#F5B057`, and deep navy/cocoa panels support the blue blanket artwork. Title/settings share the same identity and remain readable without the artwork. No baked-in title lettering is required.

The cover is `Assets/Resources/Branding/Cover.png`. It is loaded from Unity Resources, bundled with builds, and cropped to fill the viewport. The dark left overlay protects live menu contrast. Layouts scale to fit the screen. The illustration is an original concept of Marty, Lakitu and Rio, not a verified likeness of the real animals or house.

## Front-end behavior

- Gameplay and its HUD remain paused/hidden on the title.
- Start/Continue resumes current loaded progress. New Adventure requires a second selection when progress exists, resets missions/friendship and Marty's spawn, and preserves settings.
- Settings is accessible before starting and during gameplay. Closing title settings returns to the paused title. Return to Title is available from gameplay settings.
- Mouse, arrows/left stick and Enter/controller face buttons operate menus. Escape returns from information pages, cancels rebinding, or closes settings. Gameplay input waits for menu buttons to be released after resuming.
- How to Play and the gameplay HUD show current bindings, using the selected Xbox or PlayStation button names. Changing a preset also changes those labels; custom button assignments keep the chosen naming style.
- How to Play and About are available on the title. Quit closes standalone builds; Unity Editor ignores Application.Quit.

## Artwork provenance and final prompt

Created on 2026-10-28 with the **built-in image_gen tool**, then copied into the project. No third-party artwork, character brand, or user photo was used. Source PNG: 1672 × 941. Artwork is concept material; imported character models remain a separate task.

Prompt used:

> Use case: illustration-story. Asset type: landscape 16:9 cover artwork for the cozy original indie game Codename .... Cheweenie. A small warm tan Chiweenie with a long dachshund body, short legs, expressive dark eyes and soft floppy ears nestles under a blue blanket in a homemade blanket fort. Two friendly cats nearby: one charcoal gray and one pale cream/tan. Warm indoor lamplight, inviting soft fabric textures, storybook painterly game illustration with polished cinematic depth, charming expressive animal characters. A house interior is schematic and generic, no specific real floorplan. Composition: subjects and cozy blanket fort occupy the right two-thirds, the left third is softly shadowed quiet negative space for a live title and menu overlay. Landscape widescreen composition, rich warm cocoa and amber light with blue blanket accents. No text, no letters, no logo, no watermark, no UI, no existing characters or brands. This is original concept illustration, not a claim of the real animals' exact likeness.

Cover resource inclusion and title/settings pause transitions now pass the headless Unity smoke test. Rendered layout, physical controller navigation and Windows build appearance still require manual acceptance; headless checks do not verify visual layout.
