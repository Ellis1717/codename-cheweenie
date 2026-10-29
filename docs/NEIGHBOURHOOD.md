# Neighbourhood sniff walk

The east garden fence now has a six-metre opening at the signed walking path. Walk east through it at any time: the neighbourhood does not require completing the home or backyard missions. Follow the amber Marty dot on the map at the top right.

Explore a quiet pedestrian street with crossings, three colourful houses, a mailbox, flower beds, benches, trees and a small park pond. Sniff/greet the **M** mailbox, **G** garden and **P** park markers with E / X / Square in any order. After collecting all three stamps, greet the welcome-home marker just outside the garden gate to complete the patrol. You can continue exploring or walk back inside to the blanket fort.

This is a fictional schematic block, not a reconstruction of the family's actual neighbourhood. It contains no real addresses, private photos or inferred neighbour details. Landmark names and positions are defined in `NeighbourhoodBuilder`; family-supplied details can replace them. There are no moving cars or combat.

`SaveData.neighbourhoodState` is an additive v1 field. Older saves begin at zero without changing home quests, backyard progress, friendship, bindings or volumes. Flags are mailbox 1, garden 2, park 4 and returned home 8. Restoring strips unknown flags and a premature home flag. New Adventure resets the walk while preserving settings.

Verification: native tests cover all six visit orders, duplicate/invalid visits, old-save compatibility, JSON round-trip, quest routing, the open gate, ground coverage and reset. The release-player acceptance route walks out through the actual gate, visits each stop, returns to the gate and walks back inside using production controller input. See [validation](VALIDATION.md) for final results and screenshots. Physical controller hardware remains a human check.
