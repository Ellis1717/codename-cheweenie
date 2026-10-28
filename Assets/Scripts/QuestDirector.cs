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
            "All missions completed! Marty Party!"
        };
        private Transform player;
        private string notice = "Welcome home, Marty McFly!";
        private float noticeUntil = 8f;
        private QuestProgress progress = new QuestProgress();
        public int CurrentStage => progress.Stage;
        public void Restore(int stage) { progress = new QuestProgress(stage); }
        public void SetPlayer(Transform t) { player = t; }
        public void Announce(string message) { notice = message; noticeUntil = Time.time + 5f; }
        public void Activate(InteractableTarget target, InteractionKind kind = InteractionKind.Sniff)
        {
            if (!target || !player) return;
            var dog = player.GetComponent<MartyController>();
            float range = dog && dog.tuning ? dog.tuning.interactionRange : 2.8f;
            if (Vector3.Distance(player.position, target.transform.position) > range) return;
            var companion = target.GetComponent<CatCompanion>();
            if (companion && target.stage < CurrentStage)
            {
                Announce(companion.Befriend()); GameSave.Capture(); return;
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
            GetComponent<CozyFeedback>()?.Play(player.position);
            GameSave.Capture();
        }
        private void OnGUI()
        {
            var frontEnd = GetComponent<FrontEndMenu>(); if (frontEnd && frontEnd.ShowingTitle) return;
            var settings = GetComponent<CozySettings>(); if (settings && settings.IsOpen) return;
            var style = new GUIStyle(GUI.skin.box) { alignment = TextAnchor.MiddleLeft, fontSize = 18, wordWrap = true };
            GUI.Box(new Rect(14, 14, Mathf.Min(620, Screen.width - 28), 94),
                "CODENAME .... CHEWEENIE\nMission " + Mathf.Min(CurrentStage + 1, 4) + ": " + objectives[CurrentStage], style);
            if (Time.time < noticeUntil)
                GUI.Box(new Rect(14, 114, Mathf.Min(620, Screen.width - 28), 52), notice, style);
            int row = 0;
            foreach (var cat in Object.FindObjectsByType<CatCompanion>(FindObjectsSortMode.None))
                GUI.Box(new Rect(14, 176 + row++ * 46, Mathf.Min(620, Screen.width - 28), 42), cat.CompanionObjective + " | Friendship " + cat.Friendship + "/3", style);
            GUI.Box(new Rect(14, Screen.height - 58, Mathf.Min(770, Screen.width - 28), 44),
                "WASD/Stick move | E/X sniff | Space/A jump | Shift/B sprint | C/Y cozy | B/LB between | Esc/Start settings", style);
        }
    }
}
