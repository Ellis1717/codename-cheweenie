# Retained Lakitu and Rio characters

Both cats now load saved prefabs from `Assets/Resources/Cats`, with retained mesh assets, editable materials, UV coat PNGs and CatModelRig joints. The supplied archive contained cat photos, not cat meshes; these are original stylized models guided by those references.

Lakitu/Keetz has the mostly white front face and forehead, asymmetric black patch over his left eye (viewer right), black ears/back, pink nose, white chest/paws and collar/tag. Rio/Sis/Miss Priss has the mostly black head/body, white chin and bib, dark nose and black paws. Small unseen markings, eye colour and anatomy remain approximations.

The four articulated legs, tail and blinking are driven by `CatModelRig` through `CatVisual`. The saved models contain no colliders, Rigidbody or companion/save logic. Each original root keeps its CatCompanion, collider, kinematic Rigidbody and identity. A procedural fallback is available when the resource is absent, but normal builds load the retained prefabs.

Edit the prefabs, materials and coat textures in Unity. **Cheweenie > Art > Create or rebuild retained cat characters** regenerates geometry, joints and coat paint from code; back up hand-painted coats before explicitly regenerating them. Normal Windows builds keep existing prefabs/material/texture edits. Source: CatModelBuilder, CatVisual and RetainedArtWriter. Commit all assets and `.meta` files together.

The original photos remain outside Assets in ignored PrivateReferences. Their people/household backgrounds and personal tag numbers are not copied into game resources or published.
