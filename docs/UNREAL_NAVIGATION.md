# Navigation and feature controls — Unreal 0.8

Open the map with **M / D-pad Up**. Select a destination with arrows/D-pad and Enter/A-Cross; B-Circle closes it. The map numbers match the destination list, which scrolls to keep the selected row visible. Choosing a destination guides Marty rather than teleporting him. The existing walking-tour boards remain available.

The local map stays north-up. Gold marks Marty's actual body heading, blue marks the actual camera view, and X marks the destination (clamped to the edge when distant). The area map fits the current indoor or outdoor area. The compass follows Marty rather than the camera; current-location text recognizes headquarters, kitchen, sofa corner, front garden, park, school approach and the nearest named street segment. Distances are game-world metres and are not surveyed real-world walking distances.

Turn instructions follow Marty's body. If you have orbited the camera around him, R / RB-R1 recenters it behind him before following those instructions. Keyboard arrow-key movement remains available; only the controller D-pad Up opens the map during play.

Destination guidance reports distance, arrival and a turn relative to Marty's heading. Selecting an outdoor destination indoors first guides him to the exit; tracking an indoor destination outdoors guides him back to the headquarters entrance. The current-objective choice follows snack/cat/fort progress automatically. Home, park, school, kitchen, companions and the editable neighbour slots are also selectable. Destination choice saves.

Mint lines follow the largest connected street/path network using a shortest-path search. Detached decorative driveways stay visible without trapping routing in a disconnected island. The approach and final links are direct bearings. Grey lines mean direct bearing only. This is a navigation guide, not a collision-tested walking route or automatic movement: houses, furniture and other obstacles still require steering. Route data refreshes about once a second and on destination/door/home changes.

The retained local street source uses east-positive x and north-positive z. The Unreal loader now maps that to east-positive X and north-negative Y, matching Unreal's yaw/input convention. The map projection, heading and left/right instructions use the same conversion. The private source file remains unchanged and outside Git. Public fallback roads and house slots remain schematic. Door arrival aligns body and camera and leaves room behind Marty for the follow camera; tour arrival faces the next stop. Original outdoor lights receive distinct forward-shading priorities to avoid the on-screen competing-light warning.

## Switches

Use **Pause → Controls & cameras**, then choose a feature page. Arrows/D-pad select; Enter/A-Cross or left/right toggles the selected switch. Mouse rows also toggle. B-Circle returns to Controls. Long lists scroll with selection. Choices save independently of mission progress and survive a new patrol. Older saves start with every optional feature enabled and standard body controls.

| Page | Individually enabled/disabled |
|---|---|
| Navigation | Map display; compass/player heading; destination guidance; street route guide; projected objective labels; current location |
| Pet features | Energy/tiredness; health/damage; backpack; dog tricks; jumping/sofa hop; digging/burying/recovery; picking up/carrying; kitchen meals; TV; barking; jogging/running/hurry |
| Camera, display & sound | Mouse/stick camera orbit; camera presets; action cameras; control hints; detailed Marty coat; music; effects |

Standard/two-stick body controls can also be switched in Controls or the Tricks menu. In body mode the right stick remains a body input independently of the camera-orbit switch. The sound-volume page retains its separate saved volume levels; turning music/effects off preserves those levels. Turning the detailed coat off selects the retained smooth model live.

Disabling energy freezes its stored value and removes tiredness/costs while off; reenabling resumes from the stored value. Disabling health prevents damage/recovery and hides its bar. Disabling backpack/carry/dig preserves inventory, carried and buried items; dropping an already carried item remains available. Reenable the relevant feature to retrieve stored/buried possessions. Disabling an active pet feature cancels the transient action safely. Turning action cameras off restores the follow view. Basic movement, interaction, pause, saving and home recovery remain available so settings cannot strand the player. World-label and hint switches affect HUD overlays; authored world signs remain scenery.

## Extension points

CozyNavigation owns destinations, location, bearings and route search; NavigationHUD owns map drawing and clipping. Neighbourhood exposes runtime street geometry and names without copying private data into Content. CozyFeatures owns labels/grouping and feature preferences. Saved feature IDs are append-only; new features should be appended before Count. DogLife/DogTricks and MartyCharacter gate the actual actions as well as UI visibility.

TEST_NAVIGATION.ps1 exercises the real map with synthetic keyboard/gamepad input, menu/position fixtures, live switches and a second-process reload. Use -Render for actual 1280×720 captures, -GameExe for a standalone build, and -MapFile with the retained local file for the separate headless private routing audit. It never records private coordinates. Physical controller feel remains an owner check.

Final 0.8 results: Editor/Game builds and packaging passed. The standalone package passed 193 assertions, including 58 navigation/feature and four preference-reload checks. Six private-routing, four private-map and sixteen legacy movement assertions bring the total to 219. Three audio waveform checks and a full-HD coat check passed too. The five final navigation/menu captures were inspected; logs contained no Error/Fatal categories or material fallbacks. [Actual build/results](evidence/ue5-08-build.json), [navigation assertions](evidence/ue5-08-navigation.json), [private audit without coordinates](evidence/ue5-08-navigation-private.json), [area map](screenshots/ue5-08-area-map.png), [switches](screenshots/ue5-08-navigation-switches.png).
