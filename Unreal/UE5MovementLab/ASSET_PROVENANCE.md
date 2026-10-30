# Retained adventure assets

- Content/Adventure/Audio contains six SoundWave assets imported from the project's original synthesized WAVs under Assets/Resources/Audio: Home, Menu, Discovery, Lakitu, Rio and Cozy. The generator is Assets/Scripts/OriginalAudio.cs; import integration is Assets/Editor/OriginalAudioImport.cs. Home is looped music; the others are effects. No third-party music was added.
- Content/Adventure/Art/Cover is imported from Assets/Resources/Branding/Cover.png, the retained project illustration. The title screen labels it as illustration, not an in-game character render.
- Source hashes and asset paths are recorded in docs/evidence/ue5-adventure-assets.json. IMPORT_AUDIO.ps1 and Scripts/import_adventure_audio.py reproduce missing imports without overwriting existing Unreal edits.
- Gameplay retains the comparison meshes/materials and original exports. New scene dressing uses engine primitives and project materials. Raw private photos, personal map data and home images were not copied.
- Keep future assets under Content/Adventure and retain original sources plus appropriate license/provenance. Generated comparison copies, caches, saves and builds are excluded from Git.
