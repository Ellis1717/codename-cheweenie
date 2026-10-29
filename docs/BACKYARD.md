# M6: backyard adventure

This is an original schematic play space, not a reconstruction of the user's real backyard. All geometry is generated from built-in primitives by `BackyardBuilder`; there are no art downloads or paid dependencies.

## Play path

Complete the original home mission, including Cheeween Cozy at the fort. A backyard objective then appears. Greet Lakitu or Rio for contextual hints. Use the existing sniff/interact action to explore three landmarks in any order:

| Landmark | Position | Route |
| --- | --- | --- |
| Stepping-stone scent trail | X -4, Z -11.5 | Short optional hops southwest of the original sniffing zone; ground-level access remains possible. |
| Raised lookout | X -10, Z -15 | Gentle ramp leads onto a railed platform; jumping is optional. |
| Secret paw passage | X 11, Z -13 | Enter the low hedge tunnel from either open end. Its centre cannot be sniffed through a side wall. |
| Ribbon cache | X 14.5, Z -16.5 | Beyond the far end of the hedge passage. Requires all three landmarks. |

Bring the ribbon back to Rio and greet her to finish. The original quest stays at completed stage 4; the backyard has separate progression. Markers are gray while locked, amber when available and green after discovery. The HUD lists remaining landmarks and shows a nearby interaction prompt using current bindings. Sight-line checks also apply to the original interactions.

## Save compatibility

`SaveData.backyardState` is an additive integer field in the existing v1 save. Older saves load it as zero, preserving home missions, settings and friendships. Route flags are 1, 2 and 4; ribbon collected is 8; ribbon returned is 16. All routes are required before the ribbon, and the ribbon is required before return. Restoration removes unknown bits and impossible dependencies; negative values become zero. Backyard state is cleared if the restored home quest is incomplete. New Adventure clears this progress along with the home quest and cat state.

## Verification

The standalone suite executes 76 backyard checks, covering all six route orders, locked discovery, prerequisites, duplicate attempts, invalid flags and partial save restoration. Together with existing quest/input/audio checks, **145 standalone checks pass**. Runtime/editor/test sources compile against installed Unity 6.3 assemblies with zero warnings/errors.

The extended Unity smoke test now **passes in headless PlayMode**. It checks pre-unlock rejection, premature cache rejection, hedge wall sight lines, all discoveries, delivery to Rio, captured save flags, preservation of the home quest and New Adventure reset. It uses synchronized teleports for quest interactions. Full manual traversal, camera clearance, rendered layout, physical controller play and cross-session save behavior still need verification.

Acceptance route: finish home mission; visit landmarks in each preferred order; walk up/down the lookout ramp; jump the stepping stones; walk both directions through the tunnel; attempt to sniff through the hedge wall; collect/deliver the ribbon; restart and inspect saved progress; start New Adventure and verify every marker resets. The original sniffing marker and house entrance must remain reachable throughout.
