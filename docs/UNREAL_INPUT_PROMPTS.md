# Codename .... Cheweenie — input-aware directions

Development source, November 6, 2026. Select **Controls & cameras → Controller display → Auto** to follow the latest meaningful input. Xbox prompts use A/B/X/Y; PlayStation uses Cross/Circle/Square/Triangle; unknown controllers use positional button names. Keyboard input restores keyboard instructions. Saved manual display overrides still apply.

Gym strength, agility and speed signs, and both garage/gym door signs, now keep their original input phrases and refresh when the displayed input family changes. This refresh also runs while a pause menu is open. The bed's get-up prompt, trick hint, keyboard-worded menu footer, and abbreviated room movement/button hints use the shared formatter. Formatting scans the original string once so replacements cannot be converted again within the same call.

The Help page uses the same input phrases, including controller menu/settings navigation and room pet selection. Its keyboard direct-trick instructions correctly show Left Ctrl + 1/2/3/4. Unmapped keys such as Print Screen, Windows keys and Windows-key chords do not select keyboard prompts. This changes presentation and test fixtures; gameplay bindings are retained.

Run `Unreal/UE5MovementLab/BUILD.ps1`, `TEST_CONTROLLER_GUIDE.ps1 -Render`, and `TEST.ps1`. The guide suite runs the real game map with synthetic Slate events and simulated device descriptors. It checks device swaps, exact footer text, Help instructions, unknown hardware, screenshot keys, manual override, drift rejection and actual world sign text while paused. Eight fresh screenshots cover the diagrams, pause screens, gym signage and controller Help page. Tests isolate preferences and disable physical controller polling.

These changes have not been packaged into the desktop release. Physical controller identification/gameplay remains unverified. The archived Unity game and independent content-only comparison preview were not modified.

## Actual verification

Both final native Editor builds passed with Unreal Engine 5.8.4. On the isolated main-based review branch, **31 rendered prompt assertions and 16 headless movement assertions passed**. On the accumulated development checkout, **36 rendered prompt assertions and 20 headless movement assertions passed**. The development suite includes five additional keyboard-diagram checks already present there. All 103 final assertion executions passed.

An initial incremental compile referenced an unavailable `EKeys::PrintScreen` constant; the final implementation filters unmapped keys instead. A first cold rendered startup was interrupted before the gameplay map and is not counted as a gameplay pass. The initial development movement render passed 19/20 checks and failed its RB recenter assertion: stick release and RB were injected together. The fixture now sends neutral stick input for 0.15 seconds before RB and disables physical polling. The exact yaw/pitch assertion is retained; the final isolated rerun passed 20/20. Gameplay camera behavior and bindings were not changed.

Reports: [review prompts](evidence/ue5-input-prompts-review.json), [review movement](evidence/ue5-input-prompts-review-movement.json), [development prompts](evidence/ue5-input-prompts-development.json), [development movement](evidence/ue5-input-prompts-development-movement.json), [initial development movement failure](evidence/ue5-input-prompts-initial-movement.json).

Five selected review-branch captures were visually checked: [Xbox pause](screenshots/ue5-input-prompts-pause-xbox.png), [PlayStation pause](screenshots/ue5-input-prompts-pause-playstation.png), [keyboard pause](screenshots/ue5-input-prompts-pause-keyboard.png), [PlayStation gym signs](screenshots/ue5-input-prompts-gym-playstation.png), [Xbox Help](screenshots/ue5-input-prompts-help-xbox.png). The accumulated checkout's inlaid Help and PlayStation pause screens were also reviewed, with Marty and the cats unobstructed.
