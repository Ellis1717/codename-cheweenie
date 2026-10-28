# Architecture

- `CheweenieBootstrap`: creates a playable prototype world during Unity runtime initialization.
- `MartyController`: third-person movement, jump, follow-camera, gamepad and keyboard mapping.
- `QuestDirector`: HUD and ordered objectives.
- `InteractableTarget`: proximity interaction for cats, backyard marker and blanket fort.
- World props and characters are placeholders generated with built-in Unity primitives. Asset replacement should not invalidate gameplay.

## Prototype objective loop
1. Find Lakitu ("Keetz").
2. Find Rio ("Sis").
3. Explore the backyard.
4. Snuggle into the blanket fort using Cheeween Cozy.

Input uses Unity legacy Input APIs for a dependency-free bootstrap. Introduce Unity Input System action maps, input rebinding and accessibility in a later iteration.
