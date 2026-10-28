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
        public int CurrentStage { get; private set; }
        public void SetPlayer(Transform t) { player = t; }
        public void Announce(string message) { notice = message; noticeUntil = Time.time + 5f; }
        public void Activate(InteractableTarget target)
        {
            if (target.stage != CurrentStage)
            {
                Announce("Marty Man has another mission first!");
                return;
            }
            CurrentStage = Mathf.Min(CurrentStage + 1, objectives.Length - 1);
            Announce(target.dialogue);
        }
        private void OnGUI()
        {
            var style = new GUIStyle(GUI.skin.box) { alignment = TextAnchor.MiddleLeft, fontSize = 18, wordWrap = true };
            GUI.Box(new Rect(14, 14, Mathf.Min(620, Screen.width - 28), 94),
                "CODENAME .... CHEWEENIE\nMission " + Mathf.Min(CurrentStage + 1, 4) + ": " + objectives[CurrentStage], style);
            if (Time.time < noticeUntil)
                GUI.Box(new Rect(14, 114, Mathf.Min(620, Screen.width - 28), 52), notice, style);
            GUI.Box(new Rect(14, Screen.height - 58, Mathf.Min(770, Screen.width - 28), 44),
                "WASD/Stick move | E/X interact | Space/A jump | Shift/B sprint | C/Y cozy", style);
        }
    }
}
