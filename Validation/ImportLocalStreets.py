"""Build a private local Unity map from an OSM XML extract and a local geocode JSON file.

The output deliberately contains no civic number, address string, latitude or longitude.
It still identifies a home relative to public streets: keep the output out of public Git.
Map data is derived from OpenStreetMap contributors under ODbL 1.0.
"""
import argparse
import heapq
import json
import math
import pathlib
import xml.etree.ElementTree as ET


def export(osm_path, home_path, output_path, school_path=None):
    home = json.loads(pathlib.Path(home_path).read_text(encoding="utf-8-sig"))
    if isinstance(home, list):
        home = home[0]
    lat, lon = float(home["lat"]), float(home["lon"])
    scale = 1.5
    root = ET.parse(osm_path).getroot()
    points = {}
    for node in root.findall("node"):
        x = (float(node.attrib["lon"]) - lon) * 111320 * math.cos(math.radians(lat))
        z = (float(node.attrib["lat"]) - lat) * 111320
        if math.hypot(x, z) < 1100:
            points[node.attrib["id"]] = (x * scale, z * scale)
    graph, roads, parks = {}, [], []
    home_street = home.get("display_name", "").split(",")[1].strip()
    for way in root.findall("way"):
        tags = {tag.attrib["k"]: tag.attrib["v"] for tag in way.findall("tag")}
        refs = [ref.attrib["ref"] for ref in way.findall("nd")]
        if tags.get("leisure") == "park" and tags.get("name"):
            polygon = [points[ref] for ref in refs if ref in points]
            if len(polygon) >= 3:
                parks.append((tags["name"], polygon))
        if tags.get("highway") not in {"residential", "tertiary", "tertiary_link", "secondary", "secondary_link", "unclassified", "living_street", "footway", "path", "pedestrian", "service"} or tags.get("access") == "private" or tags.get("service") == "driveway":
            continue
        chain = []
        for ref in refs + [None]:
            if ref not in points:
                if len(chain) >= 2:
                    roads.append({"name": tags.get("name", "Walking path"), "width": 3.5 if tags["highway"] in {"footway", "path"} else 8, "kind": tags["highway"], "ids": chain})
                chain = []
            else:
                chain.append(ref)
    for road in roads:
        for a, b in zip(road["ids"], road["ids"][1:]):
            weight = math.dist(points[a], points[b])
            graph.setdefault(a, {})[b] = weight
            graph.setdefault(b, {})[a] = weight

    def projection(p, a, b):
        dx, dz = b[0] - a[0], b[1] - a[1]
        length = dx * dx + dz * dz
        t = max(0, min(1, ((p[0] - a[0]) * dx + (p[1] - a[1]) * dz) / length)) if length else 0
        return a[0] + t * dx, a[1] + t * dz

    def split(road, index, p):
        a, b = road["ids"][index:index + 2]
        if math.dist(points[a], p) < .02: return a
        if math.dist(points[b], p) < .02: return b
        ref = "local_join_" + str(len(points))
        points[ref] = p
        road["ids"].insert(index + 1, ref)
        graph[a].pop(b, None); graph[b].pop(a, None)
        for start, end in ((a, ref), (ref, b)):
            weight = math.dist(points[start], points[end])
            graph.setdefault(start, {})[end] = weight; graph.setdefault(end, {})[start] = weight
        return ref

    # OSM sidewalks may end beside a road without sharing its centre-line node.
    # Short game walking connectors join those public approaches; private driveways stay excluded.
    for walkway in list(roads):
        if walkway["kind"] not in {"footway", "path", "service"}: continue
        for endpoint in (walkway["ids"][0], walkway["ids"][-1]):
            best = None
            for road in roads:
                if road is walkway or road["kind"] == "connector" or endpoint in road["ids"]: continue
                for index, (a, b) in enumerate(zip(road["ids"], road["ids"][1:])):
                    p = projection(points[endpoint], points[a], points[b]); gap = math.dist(points[endpoint], p)
                    limit = 12 if road["kind"] in {"footway", "path", "service"} else 24
                    if gap < limit and (best is None or gap < best[0]): best = gap, road, index, p
            if best is not None:
                _, road, index, p = best; join = split(road, index, p)
                if join != endpoint:
                    graph.setdefault(endpoint, {})[join] = math.dist(points[endpoint], p)
                    graph.setdefault(join, {})[endpoint] = math.dist(points[endpoint], p)
                    roads.append({"name": "Walking path", "width": 3.5, "kind": "connector", "ids": [endpoint, join]})

    home_nodes = {ref for road in roads if road["name"].casefold() == home_street.casefold() for ref in road["ids"]}
    if not home_nodes:
        raise ValueError("The geocoded home street is absent from the map extract")
    entry = min(home_nodes, key=lambda ref: math.hypot(*points[ref]))

    def shortest(start):
        distance, previous, queue = {start: 0}, {}, [(0, start)]
        while queue:
            cost, current = heapq.heappop(queue)
            if cost != distance[current]:
                continue
            for nxt, length in graph.get(current, {}).items():
                candidate = cost + length
                if candidate < distance.get(nxt, float("inf")):
                    distance[nxt], previous[nxt] = candidate, current
                    heapq.heappush(queue, (candidate, nxt))
        return distance, previous

    distance, _ = shortest(entry)
    # A real junction with a second named road, rather than an invented garden location.
    junctions = []
    for road in roads:
        if road["name"] in {home_street, "Walking path"}:
            continue
        for ref in road["ids"]:
            if ref in home_nodes and ref in distance:
                junctions.append((distance[ref], road["name"], ref))
    if not junctions:
        raise ValueError("No connected named junction found")
    _, junction_name, junction = min(junctions)

    def inside(point, polygon):
        x, z = point
        result = False
        for a, b in zip(polygon, polygon[1:] + polygon[:1]):
            if (a[1] > z) != (b[1] > z) and x < (b[0] - a[0]) * (z - a[1]) / (b[1] - a[1]) + a[0]:
                result = not result
        return result

    park_candidates = []
    for name, polygon in parks:
        for ref in distance:
            if inside(points[ref], polygon):
                park_candidates.append((distance[ref], name, ref, points[ref]))
            else:
                edge = min(polygon, key=lambda p: math.dist(points[ref], p))
                gap = math.dist(points[ref], edge)
                if gap < 45:
                    park_candidates.append((distance[ref] + gap, name, ref, edge))
    if not park_candidates:
        raise ValueError("No reachable named public park found in the map extract")
    _, park_name, park_node, park_point = min(park_candidates)

    def point(value):
        return {"x": round(value[0], 3), "z": round(value[1], 3)}

    tour = [{**point((0, -17)), "stop": 0}, {**point((0, -23)), "stop": 0}]
    used_nodes = {entry}
    current = entry
    tour.append({**point(points[entry]), "stop": 1})
    destinations = [(junction, 2), (park_node, 4)]
    school = None
    park_shortcut = False
    if school_path:
        school = json.loads(pathlib.Path(school_path).read_text(encoding="utf-8-sig"))
        if isinstance(school, list):
            school = school[0]
        sx = (float(school["lon"]) - lon) * 111320 * math.cos(math.radians(lat)) * scale
        sz = (float(school["lat"]) - lat) * 111320 * scale
        approaches = []
        for road in roads:
            for index, (a, b) in enumerate(zip(road["ids"], road["ids"][1:])):
                if a not in distance: continue
                p = projection((sx, sz), points[a], points[b])
                approaches.append((math.dist(p, (sx, sz)), road, index, p))
        _, approach_road, approach_index, approach = min(approaches, key=lambda item: item[0])
        school_node = split(approach_road, approach_index, approach)
        distance, _ = shortest(entry)
        if math.dist(points[school_node], (sx, sz)) > 65 * scale:
            raise ValueError(f"The extract lacks a connected school approach (nearest connected path is {math.dist(points[school_node], (sx, sz)) / scale:.1f}m from the grounds centre)")
        destinations.append((school_node, 16))
        park_shortcut = math.dist(park_point, (sx, sz)) < 500
    destinations.append((entry, 0))
    for destination, stop in destinations:
        if school and park_shortcut and stop == 16:
            # A stylized game walking path crosses the green space to the mapped school site.
            # This is a gameplay connector, not a claim about an official public entrance.
            tour.append({**point((sx, sz)), "stop": 16})
            current = school_node
            continue
        if school and park_shortcut and current == school_node:
            tour.append({**point(park_point), "stop": 0})
            current = park_node
        if current == park_node and destination != park_node:
            tour.append({**point(points[current]), "stop": 0})
        if school and current == school_node and destination != school_node:
            tour.append({**point(points[current]), "stop": 0})
        distances, previous = shortest(current)
        if destination not in distances:
            raise ValueError("Disconnected patrol destination")
        route, cursor = [], destination
        while cursor != current:
            route.append(cursor)
            cursor = previous[cursor]
        for ref in reversed(route):
            used_nodes.add(ref)
            tour.append({**point(points[ref]), "stop": stop if ref == destination and stop not in {4, 16} else 0})
        if stop == 4:
            tour.append({**point(park_point), "stop": 4})
        if stop == 16:
            tour.append({**point((sx, sz)), "stop": 16})
        current = destination
    tour += [{**point((0, -23)), "stop": 8}, {**point((0, -17)), "stop": 0}]
    connected_roads = [road for road in roads if any(ref in distance for ref in road["ids"]) and (any(ref in used_nodes for ref in road["ids"]) or (road["name"] != "Walking path" and any(math.hypot(*points[ref]) < 1000 for ref in road["ids"])))]
    public_roads = [{"name": road["name"], "width": road["width"], "points": [point(points[ref]) for ref in road["ids"]]} for road in connected_roads]
    data = {
        "version": 1,
        "attribution": "© OpenStreetMap contributors · ODbL 1.0 · https://www.openstreetmap.org/copyright",
        "roads": public_roads,
        "stops": [
            {**point(points[entry]), "stop": 1, "label": home_street + " sniff stop"},
            {**point(points[junction]), "stop": 2, "label": junction_name + " junction"},
            {**point(park_point), "stop": 4, "label": park_name},
            {**point((0, -23)), "stop": 8, "label": "Welcome Home"},
        ],
        "parks": [{"name": name, "points": [point(p) for p in polygon]} for name, polygon in parks],
        "tour": tour,
    }
    if school:
        school_name = school["display_name"].split(",")[0]
        data["stops"].insert(3, {**point((sx, sz)), "stop": 16, "label": school_name})
        data["school"] = {**point((sx, sz)), "label": school_name}
        data["roads"].append({"name": "Walking path", "width": 3.5, "points": [point(points[school_node]), point((sx, sz))]})
        if park_shortcut:
            data["roads"].append({"name": "Walking path", "width": 3.5, "points": [point(park_point), point((sx, sz))]})
        data["gameplayConnectors"] = "Home, sidewalk joins and park/school approaches are stylized game paths, not verified public access routes."
    destination = pathlib.Path(output_path)
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text(json.dumps(data, indent=2, ensure_ascii=False), encoding="utf-8")
    print(f"Exported private map: {len(public_roads)} road segments, {len(parks)} parks, {len(tour)} tour waypoints; no address or geographic coordinates in output.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("osm_xml")
    parser.add_argument("home_geocode_json")
    parser.add_argument("output_json")
    parser.add_argument("--school-json")
    args = parser.parse_args()
    export(args.osm_xml, args.home_geocode_json, args.output_json, args.school_json)
