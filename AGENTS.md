# Codex agent instructions

Project: **Codename .... Cheweenie**. Preserve the title spelling and punctuation.

## Product canon
Marty (aka Mr. Marty, Marty Man, Marty Party, Marty McFly) is a little Chiweenie named after Martin St-Louis. His signature **Cheween in Between** squeezes between people cuddling. **Cheeween Cozy** is the blanket snuggling move. Cat companions: Lakitu (Keetz / Keeterman) and Rio (Sis / Little Miss / Miss Priss).

## Engineering rules
- Target Unity 6.3 LTS, C#; prefer small, testable MonoBehaviour components.
- Keep prototype playable from an empty scene via runtime bootstrap.
- Preserve keyboard and Xbox/PlayStation gamepad controls.
- Avoid copyrighted artwork and third-party code without appropriate license.
- Never commit Library/, Temp/, Logs/, Obj/, builds or secrets.
- After each milestone: compile in Unity, run play mode, verify controller + keyboard, and update README.
- If no Unity editor is available, clearly label smoke checks unverified; don't claim gameplay tests.
- Keep the project self-contained and avoid paid dependencies.

## Next work
Implement milestones listed in docs/CODEX_HANDOFF.md in order and open focused PRs with concise test notes.
