# Cover and front-end identity

The exact game title is **Codename .... Cheweenie**. The title screen renders the wordmark as live text over reference-guided cover artwork, with a small procedural paw mark. Warm ivory `#FAEDD1`, amber `#F5B057`, and deep navy/cocoa panels support the blue blanket artwork. Title/settings share the same identity and remain readable without the artwork. No baked-in title lettering is required.

The cover is `Assets/Resources/Branding/Cover.png`. It is loaded from Unity Resources, bundled with builds, and cropped to fill the viewport. The dark left overlay protects live menu contrast. Layouts scale to fit the screen. Lakitu and Rio now follow the owner-provided black-and-white cat references. Expressions, proportions, Marty and the room remain stylized; this is not an exact animal or house reconstruction.

## Front-end behavior

- Gameplay and its HUD remain paused/hidden on the title.
- Start/Continue resumes current loaded progress. New Adventure requires a second selection when progress exists, resets missions/friendship and Marty's spawn, and preserves settings.
- Settings is accessible before starting and during gameplay. Closing title settings returns to the paused title. Return to Title is available from gameplay settings.
- Mouse, arrows/D-pad/left stick and Enter/A/Cross operate menus. Escape/B/Circle returns from information pages or closes settings. Escape/Menu/Options cancels rebinding. Gameplay input waits for menu buttons to be released after resuming.
- The Controls guide and gameplay HUD show current bindings, using the selected Xbox or PlayStation button names. Changing a preset also changes those labels; custom button assignments keep the chosen naming style.
- Controls & camera, Adventure guide, Meet the crew and About are available on the title. The field guide also opens from paused Settings; see [menu guide](MENU_GUIDE.md). Quit closes standalone builds; Unity Editor ignores Application.Quit.

## Illustrated crew cards — 0.9.0

Three new square illustrations are retained as Resources/Branding/MartyTrail, LakituPatrol and RioCozy, with PNGs and import metadata. Marty sniffs leaves in his Explorer gear, Lakitu investigates a leaf and Rio guards her blanket/ribbon. They appear as title cards and full character-page artwork. The built-in image_gen tool used the original Explorer game render and private cat photos as references; [full prompt set](MENU_ART_PROMPTS.md). Outputs were inspected for the signature colours/markings and exclusion of people, photo backgrounds, tag details and baked-in UI text. They are illustrative interpretations; in-world geometry is unchanged. Existing cover remains retained.

## Original artwork history — superseded 2026-10-29

The first version was created on 2026-10-28 with the **built-in image_gen tool**, without the owner photographs. Its gray and cream cats were incorrect placeholders. It has been superseded by the photo-guided revision below; do not reuse those gray/cream descriptions for new cat assets. Source PNG: 1672 × 941. Artwork is concept material; imported character models remain a separate task.

Prompt used:

> Use case: illustration-story. Asset type: landscape 16:9 cover artwork for the cozy original indie game Codename .... Cheweenie. A small warm tan Chiweenie with a long dachshund body, short legs, expressive dark eyes and soft floppy ears nestles under a blue blanket in a homemade blanket fort. Two friendly cats nearby: one charcoal gray and one pale cream/tan. Warm indoor lamplight, inviting soft fabric textures, storybook painterly game illustration with polished cinematic depth, charming expressive animal characters. A house interior is schematic and generic, no specific real floorplan. Composition: subjects and cozy blanket fort occupy the right two-thirds, the left third is softly shadowed quiet negative space for a live title and menu overlay. Landscape widescreen composition, rich warm cocoa and amber light with blue blanket accents. No text, no letters, no logo, no watermark, no UI, no existing characters or brands. This is original concept illustration, not a claim of the real animals' exact likeness.

The current Windows cover, menus and cat portraits are captured from the rendered player. Native tests verify asset availability and coat/physics rules; visual reference matching remains an artistic judgement rather than a likeness score. Physical controller hardware remains a human check.

## Photo-guided revision — 2026-10-29

The original cat photos were recovered from the earlier **Create Marty Game** chat. The held cat's Keetz collar tag identifies Lakitu; the darker couch cat is Rio. Local source copies are kept in ignored `PrivateReferences/Cats/Lakitu-reference.jpg` and `Rio-reference.jpg`, outside Unity Assets. Raw photos and their household/person backgrounds are not included in Git or the game.

- **Lakitu / Keetz:** black ears/crown/back patches, mostly white face/muzzle/chest/front legs, narrow white forehead blaze, asymmetric black patch over the cat's left eye (viewer right), pink nose, olive eyes and a dark collar/round tag.
- **Rio / Miss Priss:** predominantly black face/body, small white chin and broad white chest bib, white whiskers and a fuller silhouette. Rio must not inherit Lakitu's large white facial blaze or become a cream/ginger cat. The couch photo does not clearly establish every paw marking or eye detail; those small details remain stylized.

The current cover was edited with the built-in imagegen tool using the existing cover as the edit target and both original photos as identity references. The prompt preserved Marty, blue blankets, warm lighting, composition and left-side menu space, replaced only the cats with these markings, and excluded all people/photo backgrounds and baked-in lettering. The 1672 × 941 output is saved at the existing Cover resource path/GUID.

The runtime `CatVisual` uses original procedural geometry, photo-guided coat paint, pointed ears, muzzle/nose/eyes, whiskers, paws, tails, blinking and walking paws. Lakitu has a white body with black saddle patches; Rio has a black body with the white bib. Their original root colliders, Rigidbody, companion logic, quest IDs and save data are unchanged. These are recognizable stylized approximations, not photogrammetry or finished production fur assets.
