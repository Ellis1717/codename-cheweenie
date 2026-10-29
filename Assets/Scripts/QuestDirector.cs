using UnityEngine;

namespace CodenameCheweenie
{
    public sealed class QuestDirector : MonoBehaviour
    {
        private readonly string[] objectives =
        {
            "Find Lakitu (Keetz / Keeterman) in headquarters",
            "Find Rio (Sis / Little Miss / Miss Priss)",
            "Explore the backyard sniffing zone",
            "Use Cheeween Cozy at the blanket fort",
            "Home mission complete! A backyard adventure awaits."
        };
        private Transform player;
        private string notice = "Welcome home, Marty McFly!";
        private float noticeUntil = 8f;
        private QuestProgress progress = new QuestProgress();
        private CatCompanion[] cats;
        private FrontEndMenu frontEnd;
        private CozySettings settings;
        private BackyardDirector backyardHud;
        private NeighbourhoodDirector neighbourhoodHud;
        private MartyController dogHud;
        private MartyInput inputHud;
        private int controlsSignature = int.MinValue;
        private string controlsText;
        private void Start()
        {
            cats = Object.FindObjectsByType<CatCompanion>(FindObjectsSortMode.None);
            frontEnd = GetComponent<FrontEndMenu>(); settings = GetComponent<CozySettings>(); backyardHud = GetComponent<BackyardDirector>();
            neighbourhoodHud = GetComponent<NeighbourhoodDirector>();
            if (player) { dogHud = player.GetComponent<MartyController>(); inputHud = player.GetComponent<MartyInput>(); }
        }
        public int CurrentStage => progress.Stage;
        public void Restore(int stage) { progress = new QuestProgress(stage); }
        public void SetPlayer(Transform t) { player = t; }
        public void Announce(string message) { notice = message; noticeUntil = Time.time + 5f; }
        public void Activate(InteractableTarget target, InteractionKind kind = InteractionKind.Sniff)
        {
            if (!target || !player) return;
            var dog = player.GetComponent<MartyController>();
            float range = dog && dog.tuning ? dog.tuning.interactionRange : 2.8f;
            if (!target.CanReach(player, range)) return;
            if (target.neighbourhoodStop != NeighbourhoodStop.None)
            {
                if (kind == InteractionKind.Sniff) GetComponent<NeighbourhoodDirector>()?.Visit(target);
                return;
            }
            var backyard = GetComponent<BackyardDirector>();
            if (target.backyardClue != BackyardClue.None)
            {
                if (kind == InteractionKind.Sniff && backyard) backyard.Discover(target);
                return;
            }
            var companion = target.GetComponent<CatCompanion>();
            if (companion && target.stage < CurrentStage)
            {
                string greeting = companion.Befriend();
                GetComponent<GameAudio>()?.Play(companion.energetic ? SoundCue.Lakitu : SoundCue.Rio);
                Announce(backyard ? backyard.Greeting(companion, greeting) : greeting); GameSave.Capture(); return;
            }
            if (!companion && target.stage < CurrentStage && (target.stage != 3 || kind == InteractionKind.Cozy))
            {
                foreach (var cat in Object.FindObjectsByType<CatCompanion>(FindObjectsSortMode.None)) cat.CompleteAt(target.stage);
                Announce(target.dialogue); GameSave.Capture(); return;
            }
            if (!progress.TryAdvance(target.stage, kind))
            {
                Announce("Marty Man has another mission first!");
                return;
            }
            Announce(companion ? companion.Befriend() : target.dialogue);
            foreach (var cat in Object.FindObjectsByType<CatCompanion>(FindObjectsSortMode.None))
                if (cat.CompleteAt(target.stage)) Announce(cat.energetic ? "Keeterman's backyard patrol complete!" : "Miss Priss approves of Cheeween Cozy!");
            if (kind != InteractionKind.Cozy) GetComponent<CozyFeedback>()?.Play(player.position,
                companion ? companion.energetic ? SoundCue.Lakitu : SoundCue.Rio : SoundCue.Discovery);
            GameSave.Capture();
        }
        private void OnGUI()
        {
            if (frontEnd && frontEnd.ShowingTitle || settings && settings.IsOpen || !inputHud || neighbourhoodHud && neighbourhoodHud.MapExpanded) return;
            Matrix4x4 before = GUI.matrix;
            float scale = Mathf.Min(Screen.width / 1280f, Screen.height / 720f);
            float width = Screen.width / scale, height = Screen.height / scale;
            GUI.matrix = Matrix4x4.Scale(Vector3.one * scale);
            Color panel = new Color(.045f, .065f, .09f, .9f);
            BrandTheme.Fill(new Rect(18, 18, 450, 92), panel);
            GUI.Label(new Rect(32, 25, 422, 25), BrandTheme.Title, BrandTheme.Text(16, BrandTheme.Amber, true));
            GUI.Label(new Rect(32, 54, 422, 50), objectives[CurrentStage], BrandTheme.Text(18, BrandTheme.Cream, true));
            float top = 122;
            if (Time.time < noticeUntil)
            {
                BrandTheme.Fill(new Rect(18, top, 450, 66), panel);
                GUI.Label(new Rect(30, top + 6, 426, 54), notice, BrandTheme.Text(16, BrandTheme.Cream));
                top += 76;
            }
            if (CurrentStage == QuestProgress.CompletedStage && backyardHud)
            {
                BrandTheme.Fill(new Rect(18, top, 450, 105), panel);
                GUI.Label(new Rect(30, top + 6, 426, 24), "BACKYARD ADVENTURE", BrandTheme.Text(14, BrandTheme.Amber, true));
                GUI.Label(new Rect(30, top + 31, 426, 68), backyardHud.Objective, BrandTheme.Text(16, BrandTheme.Cream));
            }
            else if (cats != null)
            {
                foreach (var cat in cats)
                {
                    if (!cat) continue;
                    string name = cat.energetic ? "Lakitu" : "Rio";
                    GUI.Label(new Rect(24, top, 440, 24), name + " · Friendship " + cat.Friendship + "/3", BrandTheme.Text(14, BrandTheme.Cream));
                    top += 26;
                }
            }
            var nearby = dogHud ? dogHud.NearestTarget() : null;
            if (nearby)
            {
                BrandTheme.Fill(new Rect(18, height - 124, width - 36, 36), panel);
                GUI.Label(new Rect(30, height - 118, width - 60, 25),
                    (nearby.stage == 3 && nearby.backyardClue == BackyardClue.None ? inputHud.BindingLabel(DogAction.Cozy) : inputHud.BindingLabel(DogAction.Interact)) + ": " + nearby.name,
                    BrandTheme.Text(16, BrandTheme.Amber, true));
            }
            if (controlsSignature != inputHud.BindingSignature)
            {
                controlsSignature = inputHud.BindingSignature;
                controlsText = "Move WASD / stick · Jump " + inputHud.BindingLabel(DogAction.Jump) + " · Greet " + inputHud.BindingLabel(DogAction.Interact)
                    + " · Sprint " + inputHud.BindingLabel(DogAction.Sprint) + "\nCozy " + inputHud.BindingLabel(DogAction.Cozy)
                    + " · Between " + inputHud.BindingLabel(DogAction.Between) + " · Settings " + inputHud.BindingLabel(DogAction.Menu)
                    + "\nCamera "+((CameraPreset)GameSave.Data.cameraMode)+": V / R3 · Look: right stick / RMB drag · Recenter: Home / RB / R1";
            }
            BrandTheme.Fill(new Rect(18, height - 78, width - 36, 62), panel);
            GUI.Label(new Rect(30, height - 74, width - 60, 58), controlsText, BrandTheme.Text(14, BrandTheme.Cream));
            GUI.matrix = before;
        }
    }
}
