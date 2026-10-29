using UnityEngine;

namespace CodenameCheweenie
{
    public sealed class MappedNeighbourhoodBuilder : MonoBehaviour
    {
        private LocalMapData data;
        private readonly Color pavement = new Color(.72f, .7f, .6f), asphalt = new Color(.3f, .33f, .36f);
        public void Build(LocalMapData map)
        {
            data = map; Rect bounds = data.Bounds;
            Box("Local walking ground", new Vector3(bounds.center.x, -.36f, bounds.center.y), new Vector3(bounds.width + 40, .5f, bounds.height + 40), new Color(.3f, .53f, .3f));
            if (data.parks != null) foreach (var park in data.parks)
            {
                Vector3 min = park.points[0].Position, max = min;
                foreach (var point in park.points) { min = Vector3.Min(min, point.Position); max = Vector3.Max(max, point.Position); }
                Box(park.name + " | schematic green space", (min + max) * .5f - Vector3.up * .105f, new Vector3(Mathf.Max(1, max.x - min.x), .01f, Mathf.Max(1, max.z - min.z)), new Color(.23f, .46f, .27f));
            }
            int houses = 0;
            foreach (var road in data.roads)
            {
                for (int i = 1; i < road.points.Length; i++)
                {
                    Vector3 a = road.points[i - 1].Position, b = road.points[i].Position;
                    Segment(road.name + " pavement", a, b, road.width + 4, -.08f, .12f, pavement);
                    Segment(road.name, a, b, road.width, -.035f, .06f, asphalt);
                    Disc(a, road.width + 4, -.08f, .06f, pavement); Disc(a, road.width, -.035f, .03f, asphalt);
                    if (i == road.points.Length - 1) { Disc(b, road.width + 4, -.08f, .06f, pavement); Disc(b, road.width, -.035f, .03f, asphalt); }
                    if (houses < 48 && i % 4 == 1 && road.name != "Walking path")
                    {
                        Vector3 direction = (b - a).normalized, side = Vector3.Cross(Vector3.up, direction);
                        Vector3 home = (a + b) * .5f + side * 22;
                        if (home.magnitude > 45 && ClearOfRoads(home, 17) && ClearOfStops(home, 22)) { House(home, direction, houses++); }
                    }
                }
                if (road.name != "Walking path")
                {
                    Vector3 middle = road.points[road.points.Length / 2].Position;
                    Sign(road.name, middle + Vector3.right * (road.width * .5f + 2));
                }
            }
            Vector3 gate = new Vector3(0, 0, -23), entry = data.Find(NeighbourhoodStop.Mailbox).Position;
            Segment("Home garden walking path", new Vector3(0, 0, -17), gate, 3, -.08f, .12f, pavement);
            Segment("Home-to-street walking path", gate, entry, 3, -.08f, .12f, pavement);
            Sign("HOME", new Vector3(3, 0, -21));
            foreach (var stop in data.stops)
            {
                var go = GameObject.CreatePrimitive(PrimitiveType.Sphere); go.name = stop.label;
                go.transform.SetParent(transform, false); go.transform.position = stop.Position + Vector3.up * .65f; go.transform.localScale = Vector3.one * .55f;
                go.GetComponent<Collider>().enabled = false; go.GetComponent<Renderer>().sharedMaterial = WorldMaterials.For(BrandTheme.Amber);
                var target = go.AddComponent<InteractableTarget>(); target.stage = -1; target.neighbourhoodStop = (NeighbourhoodStop)stop.stop;
                target.dialogue = stop.stop == 8 ? "Home again! Marty finished the local walking patrol. Time for a cozy rest." : stop.label + " stamp found! Another stop on Marty's neighbourhood walk.";
                go.AddComponent<NeighbourhoodMarker>().stop = target.neighbourhoodStop;
                Sign(stop.label, stop.Position + Vector3.right * 3);
            }
            if (data.school != null)
            {
                Vector3 position = data.school.Position, marker = data.Find(NeighbourhoodStop.School).Position;
                Vector3 away = position - marker; if (away.sqrMagnitude < 1) away = Vector3.forward;
                if (away.magnitude < 22) position = marker + away.normalized * 22;
                if (!ClearOfRoads(position, 17))
                    foreach (var side in new[] { Vector3.forward, Vector3.right, Vector3.back, Vector3.left })
                    { Vector3 candidate = marker + side * 26; if (ClearOfRoads(candidate, 17) && ClearOfStops(candidate, 20)) { position = candidate; break; } }
                Box("School | stylized exterior", position + Vector3.up * 2.4f, new Vector3(22, 5, 12), new Color(.62f, .37f, .27f));
                Box("School roof", position + Vector3.up * 5, new Vector3(23, .3f, 13), new Color(.33f, .35f, .38f));
                Sign(data.school.label, position + Vector3.back * 7);
                for (int i = -3; i <= 3; i++) Box("School window", position + new Vector3(i * 2.8f, 2.5f, -6.03f), new Vector3(1.6f, 1.5f, .04f), new Color(.67f, .83f, .87f), false);
            }
        }
        private bool ClearOfStops(Vector3 position, float radius)
        { foreach (var stop in data.stops) if (Vector3.Distance(stop.Position, position) < radius) return false; return true; }
        private bool ClearOfRoads(Vector3 position, float radius)
        {
            foreach (var road in data.roads) for (int i = 1; i < road.points.Length; i++)
            {
                Vector3 a = road.points[i - 1].Position, delta = road.points[i].Position - a;
                Vector3 nearest = a + delta * Mathf.Clamp01(Vector3.Dot(position - a, delta) / Mathf.Max(.001f, delta.sqrMagnitude));
                if (Vector3.Distance(nearest, position) < radius) return false;
            }
            return true;
        }
        private void House(Vector3 position, Vector3 direction, int index)
        {
            Color color = index % 2 == 0 ? new Color(.76f, .63f, .5f) : new Color(.53f, .65f, .73f);
            var body = Box("Neighbour house | stylized", position + Vector3.up * 2, new Vector3(8, 4.2f, 7), color); body.transform.rotation = Quaternion.LookRotation(direction);
            var roof = Box("Neighbour roof", position + Vector3.up * 4.2f, new Vector3(9, .5f, 8), new Color(.4f, .3f, .3f), false); roof.transform.rotation = body.transform.rotation;
        }
        private void Segment(string label, Vector3 a, Vector3 b, float width, float y, float height, Color color)
        {
            Vector3 delta = b - a; if (delta.sqrMagnitude < .01f) return;
            var go = Box(label, (a + b) * .5f + Vector3.up * y, new Vector3(width, height, delta.magnitude + .1f), color);
            go.transform.rotation = Quaternion.LookRotation(delta);
        }
        private void Disc(Vector3 point, float width, float y, float height, Color color)
        {
            var go = GameObject.CreatePrimitive(PrimitiveType.Cylinder); go.name = "Walkable road join";
            go.transform.SetParent(transform, false); go.transform.position = point + Vector3.up * y; go.transform.localScale = new Vector3(width, height, width);
            go.GetComponent<Collider>().enabled = false; // Cylinder primitives use a capsule collider that cannot flatten into a road surface.
            go.GetComponent<Renderer>().sharedMaterial = WorldMaterials.For(color);
        }
        private GameObject Box(string label, Vector3 position, Vector3 scale, Color color, bool solid = true)
        {
            var go = GameObject.CreatePrimitive(PrimitiveType.Cube); go.name = label; go.transform.SetParent(transform, false);
            go.transform.position = position; go.transform.localScale = scale; go.GetComponent<Collider>().enabled = solid;
            go.GetComponent<Renderer>().sharedMaterial = WorldMaterials.For(color); return go;
        }
        private void Sign(string text, Vector3 position)
        {
            var go = new GameObject(text); go.transform.SetParent(transform, false); go.transform.position = position + Vector3.up * 2.4f;
            var label = go.AddComponent<TextMesh>(); label.text = text; label.fontSize = 36; label.characterSize = .08f; label.anchor = TextAnchor.MiddleCenter; label.color = BrandTheme.Cream;
            label.font = Resources.GetBuiltinResource<Font>("LegacyRuntime.ttf"); go.GetComponent<MeshRenderer>().sharedMaterial = label.font.material;
            go.AddComponent<WalkingSign>();
        }
    }
}
