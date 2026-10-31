# Ratchet — welcome home

Ratchet is an optional character tribute to the owner's former family cat. The four owner-confirmed photos are A01–A04. They were earlier provisional Rio matches; the correction and separated observations are retained in pet-reference PR #28. Raw photos remain private.

Finish **The Great Cozy Patrol** to unlock Ratchet permanently. Completed older checkpoints also qualify automatically. Restarting the patrol preserves the unlock. Locked menu entries explain the requirement without travelling or changing progress.

After unlocking, choose **Ratchet: welcome home (memory walk)** from the title or pause menu. This opens a separate, offline room as Ratchet. Move with WASD/left stick, look with mouse/right stick, hurry with Shift/A-Cross, jump with Space/Y-Triangle, use the toy with E/X-Square and cycle cameras with V/R3. **F9 / View-Touchpad** returns to Marty’s title screen; Continue resumes the saved patrol. You can also select pets with 1–4 or D-pad left/right. This is a relaxed tribute visit in the existing room.

In LAN play, the **host's** saved unlock controls availability for everyone. Keyboard **4** or controller pet cycling selects Ratchet when he is unlocked and unclaimed. Clients cannot unlock him by changing their own saves. The room still supports two players, not four simultaneous players. Solo quests, outdoors, needs and inventory remain outside this room mode.

## Appearance and retained sources

Ratchet has his own original mesh, fuller black coat, bushy dark tail, white hind socks, smaller white front toe patches and visible white chin/bib/underside. Standing anatomy, exact marking boundaries and muted iris colour are inferred from resting photographs. The tufted coat is a first pass rather than a finished photoreal likeness.

`Scripts/build_ratchet.py` regenerates the retained Blender/FBX in `SourceArt/Ratchet`. `Scripts/import_ratchet.py` imports only Ratchet and his six original materials into `Content/Adventure/Ratchet`. Existing pet meshes, comparison content and Unity remain untouched. There are no paid or downloaded model dependencies.

This first playable version uses a **static cat mesh** with CharacterMovement physics. It has no articulated feline paws/spine/tail animation. Dog anatomy, canine gait and dog-needs/tricks stay disabled. The meow uses an existing generic cat cue, not a recording of Ratchet. Ratchet has no Halloween costume assigned yet, so his tribute appearance remains visible.

## Verification and rebuilding

Run `BUILD.ps1`, `IMPORT_RATCHET.ps1` and `TEST_RATCHET.ps1 -Render`. The test uses an isolated checkpoint, final-interaction fixtures, separate save reload, actual map travel, keyboard/controller input and an actual capture. `TEST_COOP.ps1 -Ratchet` exercises the host-unlocked selection; ordinary `TEST_COOP.ps1` exercises locked selection. Physical controllers and separate PCs are separate manual checks.

Final verification: native Editor/Game builds and full cook/archive passed. The standalone package passed 220 assertions: 23 Ratchet unlock/save/travel, 57 locked LAN, 55 unlocked rendered LAN, 20 costumes, 34 companions and 31 patrol/save checks. Final portrait and network captures were inspected; runtime Error/Fatal/material fallback matches were zero. The release gate records 43 immutable payload hashes. [Actual evidence](evidence/ue5-ratchet-build.json), [actual portrait](screenshots/ue5-ratchet-memory.png). Older Halloween, World, LAN and Unity packages remain separate.

Launch **Play Cheweenie Ratchet** on this PC’s desktop, or `PLAY_RATCHET.cmd` in the separate distribution. The default checkpoint and private map/labels are copied only when absent; original files remain. Host/join use `PLAY_RATCHET.cmd -Mode Host` and `PLAY_RATCHET.cmd -Mode Join -Address <host-IPv4>`.

To rebuild, pass a new folder to `PACKAGE.ps1 -OutputRoot`. Run packaged suites using `-GameExe` and a common test root with `Ratchet` (`TEST_RATCHET.ps1 -Render`), `Costumes` (`TEST_COSTUMES.ps1 -Render`), `Coop` (`TEST_COOP.ps1`), `UnlockedCoop` (`TEST_COOP.ps1 -Ratchet -Render`, after Ratchet save tests), `Companions` and `Adventure` subfolders. Run `FINALIZE_RATCHET.ps1 -PackageRoot <package-folder> -TestRoot <test-folder>` afterward, then inspect the actual captures separately.
