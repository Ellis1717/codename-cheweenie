# Menu and field guide

Version 0.9.0 adds **Controls & camera**, **Adventure guide**, **Meet the crew** and **About the demo** to Start/Continue, New Adventure, Settings and Quit. The title also has three illustrated character cards. New Adventure still confirms before clearing missions and friendships, and keeps settings.

## Open help

- Title: choose a guide section or select a character card with the mouse.
- During play: **F1** opens Controls and pauses the game.
- Paused Settings: **F1 / Y / Triangle**, or the **Controls & field guide** button.
- Guide: **Up/Down or vertical stick** changes section; **Left/Right or horizontal stick** changes page; **Enter/A/Cross** advances to the next page. Click the sidebar/tabs with the mouse too.
- **Escape/F1/B/Circle** goes back. Help opened from Settings returns there; help opened directly in play resumes play. Title help returns to the paused title.

Help blocks movement, camera utilities and map toggling. Resuming suppresses the transition input until release. Gameplay **Y/Triangle remains Cozy**; controller help is available from Settings.

If F1 is rebound to a gameplay action, that action takes priority during play. Help remains available through Settings and the title.

## Pages

| Section | Pages | Content |
| --- | --- | --- |
| Controls | Actions | Current keyboard and selected Xbox/PlayStation bindings, movement and rebinding instructions. |
| Controls | Camera views | Cozy/Explore/Low, orbit/recenter, view-relative movement, distance and shortcut conflicts. |
| Controls | Menus & recovery | Menu navigation, map, help, audio, bindings, New Adventure and Return Marty home. |
| Adventure | Home missions | Four introductory steps, both Cozy moves and the Fox/Leo cameos. |
| Adventure | Backyard safari | Three scent clues, ribbon cache, return to Rio and easy ramp routes. |
| Adventure | Neighbourhood | Gate/map/stops, park/school when included, returning home and simplified geometry. |
| Meet the crew | Marty / Lakitu / Rio | Three new illustrations, aliases, personalities, abilities and companion quests. |
| About | The demo | Title/cast, inspiration, original music, local saving and New Adventure behaviour. |

Action bindings are read from MartyInput each draw. Navigation uses named Input System controls. Text stays live and readable instead of being baked into artwork. Pages scale for the viewport; long body text can reduce to 17 reference pixels to fit its panel.

## Character artwork

New assets are [MartyTrail](../Assets/Resources/Branding/MartyTrail.png), [LakituPatrol](../Assets/Resources/Branding/LakituPatrol.png) and [RioCozy](../Assets/Resources/Branding/RioCozy.png). They use the retained Explorer render / private cat photos as identity references and form a warm 3D illustration series. Generated with the built-in image_gen tool using the imagegen skill; [full prompts and provenance](MENU_ART_PROMPTS.md).

These images are bundled menu artwork. Original 3D models, coat textures, motion profiles and cover remain separate resources. Raw family photos, people and house backgrounds are excluded from public assets. Keep PNGs, import metadata and prompts with the runtime source.

Edit layout/navigation in FrontEndMenu, guide content in GuideContent, and pause help entry in CozySettings. [Actual test results](VALIDATION.md).
