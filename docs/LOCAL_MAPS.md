# Private local street maps

The game can replace its fictional block with an offline local map. The runtime loads `Assets/Resources/PrivateNeighbourhood/streets.json` when available; otherwise the default neighbourhood remains playable. This folder, its metadata and downloaded map/geocode caches are excluded from Git and the snapshot exporter. Public code and tests contain only generic/synthetic locations.

The importer reads an OpenStreetMap XML extract and a locally saved Nominatim geocode result. An optional school geocode adds a fourth patrol stamp. Example with local files:

```text
python Validation/ImportLocalStreets.py <local-osm.xml> <local-home-geocode.json> Assets/Resources/PrivateNeighbourhood/streets.json --school-json <local-school-geocode.json>
```

No civic number, address string, latitude or longitude is written into the exported JSON. However, the home marker's position relative to named streets still identifies a home, so the resulting map and personalized build are kept local. A distributed family ZIP includes the derived JSON in `MapData` and its ODbL notice. Restore that JSON to the ignored Resources folder to reproduce the personalized Unity build.

Street polylines and park locations come from OpenStreetMap. Streets are uniformly scaled by 1.5 for play, and the world is flattened. Houses and the school exterior remain stylized models. Home access, short sidewalk joins and the park/school approach are game walking connectors, not verified public entrances or navigation routes. The mapped school stop must reach the school site rather than merely a distant road node. The importer rejects a disconnected school approach; runtime validation also rejects a school stop too far from its school site.

The mapped patrol visits the crescent sniff stop, a nearby junction, the park and the school, then returns to Home. The existing three-stamp patrol save carries its stamps into this four-stop version but requires the school before the Home completion flag can return. All home/backyard progress, friendship, volumes and bindings remain compatible. New Adventure clears patrol progress and keeps settings.

Walk through the **south garden gate** in the mapped edition. Hold Shift / B / Circle to sprint. **M / Xbox View / PlayStation Share** expands the north-up map; closing it restores the normal quest HUD. Signs and the map legend name the actual loaded stops. The full map keeps equal X/Z scale. There are no moving cars or combat.

Map attribution appears in the game. Map data is © [OpenStreetMap contributors](https://www.openstreetmap.org/copyright), under [ODbL 1.0](https://opendatacommons.org/licenses/odbl/1-0/). Source geometry was not copied from commercial map imagery. The application code and original procedural models are separate from the map database.
