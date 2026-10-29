using UnityEngine;

namespace CodenameCheweenie
{
    public sealed class NeighbourhoodDirector : MonoBehaviour
    {
        private NeighbourhoodProgress progress = new NeighbourhoodProgress();
        private QuestDirector quest;
        private FrontEndMenu frontEnd;
        private CozySettings settings;
        private Transform player;
        private bool expanded;
        public bool MapExpanded => expanded && MappedNeighbourhood.Current != null;
        public int State => progress.State;
        public bool Complete => progress.Complete;
        public bool Has(NeighbourhoodStop stop) => progress.Has(stop);
        public string Objective => MappedNeighbourhood.Current != null ? MappedObjective : Complete ? "Neighbourhood patrol complete! Keep exploring with Marty."
            : progress.Visits == 3 ? "All three stamps found! Greet the welcome-home sign at the garden gate."
            : "Neighbourhood sniff walk " + progress.Visits + "/3: visit the mailbox (M), flower garden (G) and park (P).";
        private void Start()
        {
            quest = GetComponent<QuestDirector>(); frontEnd = GetComponent<FrontEndMenu>(); settings = GetComponent<CozySettings>();
            player = FindFirstObjectByType<MartyController>()?.transform;
        }
        private string MappedObjective
        {
            get
            {
                if (Complete) return "Local walking patrol complete! Marty is home again.";
                if (progress.Visits == progress.RequiredVisits) return "All walking stamps found! Return to the Home marker at the south garden gate.";
                foreach (var stop in MappedNeighbourhood.Current.stops)
                    if (stop.stop != 8 && !Has((NeighbourhoodStop)stop.stop)) return "Local walk " + progress.Visits + "/" + progress.RequiredVisits + ". Next: " + stop.label + ".";
                return "Explore the nearby streets with Marty.";
            }
        }
        public void Restore(int state) { progress = new NeighbourhoodProgress(state, MappedNeighbourhood.Current != null && MappedNeighbourhood.Current.IncludesSchool); }
        private void Update()
        {
            if (!frontEnd || frontEnd.ShowingTitle || frontEnd.GuideOpen || frontEnd.Transitioning || settings && settings.IsOpen) return;
            if (GameControls.GetKeyDown(KeyCode.M) || GameControls.GetKeyDown(GameSave.Data.playStationLabels ? KeyCode.JoystickButton8 : KeyCode.JoystickButton6)) expanded = !expanded;
        }
        public void Visit(InteractableTarget target)
        {
            if (!progress.Visit(target.neighbourhoodStop))
            { quest.Announce(Has(target.neighbourhoodStop) ? "Marty already sniffed this stop. " + Objective : "Find all " + progress.RequiredVisits + " walking stamps before checking in at home."); return; }
            quest.Announce(target.dialogue); GetComponent<CozyFeedback>()?.Play(target.transform.position); GameSave.Capture();
        }
        private static Vector2 MapPoint(Vector3 position, Rect map)
        { return new Vector2(map.x + Mathf.InverseLerp(-18, 66, position.x) * map.width, map.yMax - Mathf.InverseLerp(-30, 24, position.z) * map.height); }
        private void Dot(Rect map, Vector3 point, Color color, string label)
        {
            Vector2 p = MapPoint(point, map); BrandTheme.Fill(new Rect(p.x - 3, p.y - 3, 7, 7), color);
            if (!string.IsNullOrEmpty(label)) GUI.Label(new Rect(p.x + 5, p.y - 10, 45, 20), label, BrandTheme.Text(12, BrandTheme.Cream, true));
        }
        private void OnGUI()
        {
            if (!player || !frontEnd || frontEnd.ShowingTitle || frontEnd.GuideOpen || settings && settings.IsOpen) return;
            Matrix4x4 previous = GUI.matrix;
            float scale = Mathf.Min(Screen.width / 1280f, Screen.height / 720f), width = Screen.width / scale;
            GUI.matrix = Matrix4x4.Scale(Vector3.one * scale);
            if (MappedNeighbourhood.Current != null)
            {
                int depth = GUI.depth; if (expanded) GUI.depth = -20;
                DrawLocalMap(width, Screen.height / scale); GUI.depth = depth; GUI.matrix = previous; return;
            }
            float x = width - 256;
            BrandTheme.Fill(new Rect(x, 18, 238, 284), new Color(.045f, .065f, .09f, .92f));
            GUI.Label(new Rect(x + 12, 25, 214, 24), "NEIGHBOURHOOD WALK", BrandTheme.Text(14, BrandTheme.Amber, true));
            Rect map = new Rect(x + 12, 58, 202, 114);
            BrandTheme.Fill(map, new Color(.16f, .28f, .2f));
            Vector2 road = MapPoint(new Vector3(32, 0, 0), map);
            BrandTheme.Fill(new Rect(road.x - 8, map.y, 16, map.height), new Color(.3f, .33f, .35f));
            Dot(map, Vector3.zero, new Color(.55f, .7f, 1), "HQ");
            Dot(map, new Vector3(27, 0, -19), Has(NeighbourhoodStop.Mailbox) ? Color.green : Color.white, "M");
            Dot(map, new Vector3(47, 0, -10), Has(NeighbourhoodStop.Garden) ? Color.green : Color.white, "G");
            Dot(map, new Vector3(23, 0, 12), Has(NeighbourhoodStop.Park) ? Color.green : Color.white, "P");
            Dot(map, player.position, BrandTheme.Amber, "");
            GUI.Label(new Rect(x + 12, 182, 214, 110), "Amber dot: Marty.\n" + (player.position.x > 17 ? Objective : "Explore anytime: east garden gate → neighbourhood. M mailbox · G garden · P park"), BrandTheme.Text(14, BrandTheme.Cream));
            GUI.matrix = previous;
        }
        private void DrawLocalMap(float width, float height)
        {
            var data = MappedNeighbourhood.Current;
            Rect panel = expanded ? new Rect((width - 880) * .5f, 24, 880, 600) : new Rect(width - 276, 18, 258, 310);
            BrandTheme.Fill(panel, new Color(.045f, .065f, .09f, .95f));
            GUI.Label(new Rect(panel.x + 12, panel.y + 8, panel.width - 24, 25), "LOCAL STREET WALK", BrandTheme.Text(16, BrandTheme.Amber, true));
            Rect map = new Rect(panel.x + 12, panel.y + 44, panel.width - 24, expanded ? 410 : 140);
            Rect world = expanded ? data.Bounds : new Rect(player.position.x - 180, player.position.z - 140, 360, 280);
            float aspect = map.width / map.height;
            if (world.width / world.height < aspect) { float extra = world.height * aspect - world.width; world.xMin -= extra * .5f; world.xMax += extra * .5f; }
            else { float extra = world.width / aspect - world.height; world.yMin -= extra * .5f; world.yMax += extra * .5f; }
            BrandTheme.Fill(map, new Color(.16f, .28f, .2f));
            GUI.BeginGroup(map);
            foreach (var road in data.roads) for (int i = 1; i < road.points.Length; i++)
                Line(Project(road.points[i - 1].Position, world, map.size), Project(road.points[i].Position, world, map.size), new Color(.7f, .72f, .7f), expanded ? 3 : 2);
            foreach (var stop in data.stops)
            {
                Vector2 p = Project(stop.Position, world, map.size);
                BrandTheme.Fill(new Rect(p.x - 3, p.y - 3, 7, 7), Has((NeighbourhoodStop)stop.stop) ? Color.green : Color.white);
                string letter = stop.stop == 16 ? "S" : stop.stop == 8 ? "H" : stop.stop == 4 ? "P" : stop.stop == 2 ? "B" : "A";
                GUI.Label(new Rect(p.x + (stop.stop == 8 ? -20 : 5), p.y - (stop.stop == 8 ? 22 : 10), 24, 20), letter, BrandTheme.Text(12, BrandTheme.Cream, true));
            }
            Vector2 dog = Project(player.position, world, map.size);
            BrandTheme.Fill(new Rect(dog.x - 4, dog.y - 4, 9, 9), BrandTheme.Amber);
            GUI.EndGroup();
            string legend = expanded ? "\nA: " + data.Find(NeighbourhoodStop.Mailbox).label + " · B: " + data.Find(NeighbourhoodStop.Garden).label + " · P: " + data.Find(NeighbourhoodStop.Park).label + (data.IncludesSchool ? " · S: " + data.Find(NeighbourhoodStop.School).label : "") + " · H: Home" : "";
            GUI.Label(new Rect(panel.x + 12, map.yMax + 8, panel.width - 24, expanded ? 94 : 106), "Amber dot: Marty. M / View-Share: map.\n" + Objective + legend, BrandTheme.Text(14, BrandTheme.Cream));
            GUI.Label(new Rect(expanded ? panel.x + 12 : 20, expanded ? panel.yMax - 27 : height - 19, expanded ? panel.width - 24 : width - 40, 20), "© OpenStreetMap contributors · ODbL · openstreetmap.org/copyright", BrandTheme.Text(11, BrandTheme.Cream));
        }
        private static Vector2 Project(Vector3 p, Rect world, Vector2 size)
        { return new Vector2((p.x - world.xMin) / world.width * size.x, (world.yMax - p.z) / world.height * size.y); }
        private static void Line(Vector2 a, Vector2 b, Color color, float thickness)
        {
            Matrix4x4 old = GUI.matrix;
            GUIUtility.RotateAroundPivot(Mathf.Atan2(b.y - a.y, b.x - a.x) * Mathf.Rad2Deg, a);
            BrandTheme.Fill(new Rect(a.x, a.y - thickness * .5f, Vector2.Distance(a, b), thickness), color);
            GUI.matrix = old;
        }
    }
}
