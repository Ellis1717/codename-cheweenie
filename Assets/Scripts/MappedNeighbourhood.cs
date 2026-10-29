using System;
using UnityEngine;

namespace CodenameCheweenie
{
    [Serializable] public class LocalMapPoint
    {
        public float x, z;
        public Vector3 Position => new Vector3(x, 0, z);
    }
    [Serializable] public sealed class LocalMapStop : LocalMapPoint { public int stop; public string label; }
    [Serializable] public sealed class LocalMapRoad { public string name; public float width = 8; public LocalMapPoint[] points; }
    [Serializable] public sealed class LocalMapArea { public string name; public LocalMapPoint[] points; }
    [Serializable] public sealed class LocalMapData
    {
        public int version;
        public string attribution;
        public LocalMapRoad[] roads;
        public LocalMapStop[] stops, tour;
        public LocalMapArea[] parks;
        public LocalMapStop school;
        public bool IncludesSchool => Find(NeighbourhoodStop.School) != null;
        public LocalMapStop Find(NeighbourhoodStop stop)
        { if (stops != null) foreach (var item in stops) if (item.stop == (int)stop) return item; return null; }
        public Rect Bounds
        {
            get
            {
                float minX = -30, maxX = 30, minZ = -30, maxZ = 30;
                foreach (var road in roads) foreach (var p in road.points)
                { minX = Mathf.Min(minX, p.x); maxX = Mathf.Max(maxX, p.x); minZ = Mathf.Min(minZ, p.z); maxZ = Mathf.Max(maxZ, p.z); }
                return Rect.MinMaxRect(minX - 20, minZ - 20, maxX + 20, maxZ + 20);
            }
        }
    }
    public static class MappedNeighbourhood
    {
        private static bool loaded;
        private static LocalMapData current;
        public static LocalMapData Current
        {
            get
            {
                if (loaded) return current;
                loaded = true;
                var asset = Resources.Load<TextAsset>("PrivateNeighbourhood/streets");
                if (asset) { try { current = Parse(asset.text); } catch (ArgumentException error) { Debug.LogWarning("Local map rejected; using the schematic neighbourhood. " + error.Message); } }
                return current;
            }
        }
        [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.SubsystemRegistration)]
        private static void Reset() { loaded = false; current = null; }
        public static LocalMapData Parse(string json)
        {
            var data = JsonUtility.FromJson<LocalMapData>(json);
            if (data == null || data.version != 1 || data.roads == null || data.roads.Length == 0 || data.roads.Length > 512 || data.stops == null || data.tour == null || data.tour.Length < 2 || data.tour.Length > 2000)
                throw new ArgumentException("Invalid map structure.");
            int count = 0, flags = 0;
            foreach (var road in data.roads)
            {
                if (road == null || road.points == null || road.points.Length < 2 || road.width < 2 || road.width > 20) throw new ArgumentException("Invalid road.");
                count += road.points.Length;
                foreach (var p in road.points) Validate(p);
            }
            if (count > 16000) throw new ArgumentException("Map is too large.");
            foreach (var stop in data.stops) { Validate(stop); if (stop.stop != 1 && stop.stop != 2 && stop.stop != 4 && stop.stop != 8 && stop.stop != 16) throw new ArgumentException("Invalid landmark."); if ((flags & stop.stop) != 0) throw new ArgumentException("Duplicate landmark."); flags |= stop.stop; }
            if ((flags & 15) != 15) throw new ArgumentException("Missing patrol stops.");
            foreach (var p in data.tour) { Validate(p); if (p.stop != 0 && (flags & p.stop) != p.stop) throw new ArgumentException("Invalid tour stop."); }
            if (data.parks != null) foreach (var park in data.parks) { if (park.points == null || park.points.Length < 3) throw new ArgumentException("Invalid park."); foreach (var p in park.points) Validate(p); }
            if (data.IncludesSchool)
            {
                if (data.school == null || string.IsNullOrWhiteSpace(data.school.label)) throw new ArgumentException("Missing mapped school site.");
                Validate(data.school);
                var stop = data.Find(NeighbourhoodStop.School);
                if (stop == null || Vector3.Distance(stop.Position, data.school.Position) > 65) throw new ArgumentException("School stop is too far from the mapped school site.");
            }
            else data.school = null; // JsonUtility can instantiate a missing nested reference with default fields.
            return data;
        }
        private static void Validate(LocalMapPoint point)
        { if (point == null || float.IsNaN(point.x) || float.IsInfinity(point.x) || float.IsNaN(point.z) || float.IsInfinity(point.z) || Mathf.Abs(point.x) > 2500 || Mathf.Abs(point.z) > 2500) throw new ArgumentException("Invalid map coordinate."); }
    }
}
