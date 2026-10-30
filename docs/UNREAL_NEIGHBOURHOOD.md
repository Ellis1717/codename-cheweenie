# Outside headquarters

Start the Unreal 0.5 demo and choose Explore the neighbourhood, or walk to the blue OUTSIDE door in headquarters and press E / Xbox X / PlayStation Square. The same button at the home doorway returns inside. H, or Return Marty home in Pause, recovers to headquarters from anywhere. Door transitions preserve the indoor patrol. Falling below the world also recovers home.

The larger exterior includes roads and pavements, front gardens, eight neighbour house/dog slots, a park with trees and a bench, and a Glen Dhu school exterior. There are no moving vehicles or combat. The house-front approximation uses the retained private reference's red brick, blue garage/entry doors, cream trim, two-storey bay and sheltered entrance. It is not a measured architectural reconstruction, and no photograph, civic number or vehicle is bundled.

Walk up to a dog and press Interact for a friendly greeting. The HUD counts neighbours met; those slot-based greetings autosave with the native adventure. Repeating a greeting does not add duplicates. Neighbour dogs are currently simple stationary placeholders. Their likeness, breeds and animation remain future work; Marty has the new skinned gait. New Patrol restarts the indoor mission and retains neighbour friendships and sound preferences.

Walking-tour boards near the house, park and school offer optional shortcuts. Approach the board and press Interact to visit the next landmark. These are explicit game travel shortcuts, not a claim that every route has been walked or that an entrance is publicly accessible.

## Your retained layout stays local

The launcher reads LocalGameData/streets.json when it exists. On this machine it is copied from the earlier ignored Unity neighbourhood resource. It is the retained OpenStreetMap layout, not traced Google Maps artwork or a live Google Maps feed. The native importer validates the existing metre coordinates and creates road/pavement segments at centimetre scale. The park and school retain their saved locations. Home/house architecture, dog lots and connectors are schematic additions. Roads are instanced rather than one actor per road segment.

The map-derived home position can identify a residence, so LocalGameData is excluded from Git, snapshots and cooked Content. The standard public package falls back to a compact schematic neighbourhood. To copy a personalized game to another computer, keep the launcher, its LocalGameData and the entire Windows package folder together; share that personalized folder only as intended. Without the local file the game remains playable in the schematic layout. Map attribution is shown at the outdoor entry: OpenStreetMap contributors, ODbL 1.0. See LOCAL_MAPS.md for original provenance and limitations.

To configure another local copy, run Unreal/UE5MovementLab/SETUP_LOCAL_NEIGHBOURHOOD.ps1 with -MapFile pointing at the retained streets.json. This copies the chosen map into ignored local data; it does not geocode or download anything. Public source and screenshots do not contain the personalized layout.

## Add your neighbours

Edit LocalGameData/NeighbourDogs.json, then relaunch. Config/NeighbourDogs.example.json is the non-private template. There are eight stable slots, 0 through 7:

    {"version":1,"dogs":[{"slot":0,"name":"Your dog's name","house":"Your chosen house label"}]}

Omitted slots keep explicit placeholder names. Duplicate slots use the last entry. Names/labels are limited to 40 characters. No addresses are needed. Slots identify friendships, so renaming a slot retains that slot's met status. The file stays outside Git; the owner can supply names in chat and have the roster edited directly. To add more than eight lots, extend the generator/roster limit in Source/CheweenieUE5Lab/Neighbourhood.cpp and rebuild.

## Development

Neighbourhood.h/.cpp owns geometry, private map loading, dog roster and door/tour interactions. CozyAdventure retains saves/menus and delegates exterior interaction; player collision/input and MartyGait remain separate. Original Unity and comparison maps are unchanged. The active native renderer retains Lumen and virtual shadows with smaller texture, surface-cache and shadow budgets after a rendered editor startup exhausted this machine's total memory commitment. No global memory/page-file settings or unrelated apps were changed.

The camera follows directly across door/tour transitions so it cannot lag kilometres behind a teleport. Outdoor lighting adds soft shadowless fill and a modest sun boost for readable shaded gardens; returning indoors restores the retained sun intensity and disables those fills. The world uses the existing atmosphere rather than adding a duplicate sky.

Run TEST_OUTSIDE.ps1 for schematic keyboard/controller input checks. Add -Render for captures, or -GameExe for the packaged executable. The test walks to and uses the real door; dog and board positions use teleported fixtures, followed by real interactions and grounded arrival checks. It uses an isolated acceptance save and forces the schematic map. Run with -MapFile LocalGameData/streets.json for a separate headless private-map validation/walking check that exports no coordinates or screenshots. Physical controller checks and subjective environment likeness remain for the owner.

The desktop launcher prefers 0.5 only after a validated.json marker exists beside its Windows folder. PACKAGE removes that marker when rebuilding; the launcher uses the retained 0.4 package meanwhile. Run the packaged gait, exterior and adventure scripts with -Render and OutputRoot under Saved/Packaged05/Gait, Outside and Adventure, inspect their screenshots, then run FINALIZE_PACKAGE.ps1. Finalization checks fresh passing reports/captures against payload timestamps and writes the marker with payload hashes. It excludes mutable Saved files. A freshly built executable can also be launched directly for development.

The final 0.5 package passed 12 exterior assertions, 14 gait assertions and 25+6 patrol/persistence assertions under DX12. Four additional packaged headless checks validated the retained private map and actual walking/recovery without exporting coordinates. The earlier sixteen movement checks passed in the editor-game at the 60 FPS cap. Functional tests initially missed visual sign placement, dark shadows and teleport camera problems; those were corrected and the final packaged captures inspected. Physical controller use, audible mix, subjective likeness and measured frame-rate performance remain unverified.
