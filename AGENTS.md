# Codex agent instructions

Project: **Codename .... Cheweenie**. Preserve the title spelling and punctuation.

## Product canon
Marty (aka Mr. Marty, Marty Man, Marty Party, Marty McFly) is a little Chiweenie named after Martin St-Louis. His signature **Cheween in Between** squeezes between people cuddling. **Cheeween Cozy** is the blanket snuggling move. Cat companions: Lakitu (Keetz / Keeterman) and Rio (Sis / Little Miss / Miss Priss).

## Engineering rules
- Active development targets Unreal Engine 5.8.4, per the owner's decision to set Unity aside. Follow docs/UNREAL_HANDOFF.md. Keep the Unity game, resources and builds intact as an archived fallback; do not delete or migrate them destructively.
- Prefer small native gameplay components and Enhanced Input. Keep the content-only UE5 comparison preview usable independently of the native movement project.
- The retained Unity fallback targets Unity 6.3 LTS/C# and keeps its empty-scene runtime bootstrap; Unity-specific requirements apply when editing that fallback.
- Preserve keyboard and Xbox/PlayStation gamepad controls.
- Avoid copyrighted artwork and third-party code without appropriate license.
- Never commit Library/, Temp/, Logs/, Obj/, builds or secrets.
- After each Unreal milestone: compile the native project, run the real game map, verify controller + keyboard, and update README with actual results. Clearly distinguish automated input from physical controller checks.
- If compilation or runtime testing is unavailable, label the affected checks unverified; do not claim gameplay tests. Rerun Unity checks only when Unity source/assets/settings are changed.
- Keep the project self-contained and avoid paid dependencies.

## Next work
Implement Unreal milestones listed in docs/UNREAL_HANDOFF.md in order and open focused PRs with concise test notes. docs/CODEX_HANDOFF.md retains the completed Unity history.
