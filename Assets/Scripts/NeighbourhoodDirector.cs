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
        public int State => progress.State;
        public bool Complete => progress.Complete;
        public bool Has(NeighbourhoodStop stop) => progress.Has(stop);
        public string Objective => Complete ? "Neighbourhood patrol complete! Keep exploring with Marty."
            : progress.Visits == 3 ? "All three stamps found! Greet the welcome-home sign at the garden gate."
            : "Neighbourhood sniff walk " + progress.Visits + "/3: visit the mailbox (M), flower garden (G) and park (P).";
        private void Start()
        {
            quest = GetComponent<QuestDirector>(); frontEnd = GetComponent<FrontEndMenu>(); settings = GetComponent<CozySettings>();
            player = FindFirstObjectByType<MartyController>()?.transform;
        }
        public void Restore(int state) { progress = new NeighbourhoodProgress(state); }
        public void Visit(InteractableTarget target)
        {
            if (!progress.Visit(target.neighbourhoodStop))
            { quest.Announce(Has(target.neighbourhoodStop) ? "Marty already sniffed this stop. " + Objective : "Find all three neighbourhood stamps before checking in at home."); return; }
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
            if (!player || !frontEnd || frontEnd.ShowingTitle || settings && settings.IsOpen) return;
            Matrix4x4 previous = GUI.matrix;
            float scale = Mathf.Min(Screen.width / 1280f, Screen.height / 720f), width = Screen.width / scale;
            GUI.matrix = Matrix4x4.Scale(Vector3.one * scale);
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
    }
}
