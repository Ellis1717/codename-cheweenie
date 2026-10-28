using UnityEngine;

namespace CodenameCheweenie
{
    [RequireComponent(typeof(QuestDirector))]
    public sealed class BackyardDirector : MonoBehaviour
    {
        private BackyardProgress progress = new BackyardProgress();
        private QuestDirector quest;
        public int State => progress.State;
        public bool Unlocked => quest && quest.CurrentStage == QuestProgress.CompletedStage;
        public bool Complete => progress.Complete;
        public string Objective
        {
            get
            {
                if (Complete) return "Ribbon delivered! Backyard adventure complete. Marty Party!";
                if (progress.Has(BackyardClue.Ribbon)) return "Bring the ribbon back to Rio in headquarters.";
                if (progress.RouteCount == 3) return "Follow the hedge passage to the ribbon cache.";
                string remaining = "";
                if (!progress.Has(BackyardClue.ScentTrail)) remaining += "stepping-stone scent trail; ";
                if (!progress.Has(BackyardClue.PawPassage)) remaining += "secret hedge passage; ";
                if (!progress.Has(BackyardClue.Lookout)) remaining += "raised lookout; ";
                return "Keeterman's scent safari " + progress.RouteCount + "/3: " + remaining.TrimEnd(' ', ';') + ".";
            }
        }
        private void Awake() { quest = GetComponent<QuestDirector>(); }
        public void Restore(int saved) { progress = new BackyardProgress(saved); }
        public bool Has(BackyardClue clue) => progress.Has(clue);
        public bool Available(BackyardClue clue) => progress.Available(clue, Unlocked);
        public void Discover(InteractableTarget target)
        {
            if (!Unlocked) { quest.Announce("First finish the home mission and get Cheeween Cozy. Then the backyard adventure begins!"); return; }
            if (!progress.TryDiscover(target.backyardClue, true))
            {
                quest.Announce(Has(target.backyardClue) ? "Already explored. " + Objective : "Follow all three scent clues before opening the ribbon cache."); return;
            }
            quest.Announce(target.dialogue);
            GetComponent<CozyFeedback>()?.Play(target.transform.position); GameSave.Capture();
        }
        public string Greeting(CatCompanion cat, string ordinary)
        {
            if (!Unlocked) return ordinary;
            if (!cat.energetic && progress.TryDiscover(BackyardClue.Returned, true))
            {
                GetComponent<CozyFeedback>()?.Play(cat.transform.position);
                return "Rio: My blanket ribbon! A little crooked... perfectly acceptable. Thank you, Marty.";
            }
            if (Complete) return cat.energetic ? "Keeterman: Secret patrol partners forever!" : "Rio: The blanket is just right. You may share it, Marty.";
            if (cat.energetic) return "Keeterman: Sniff the stepping stones, squeeze through the hedge passage, and check the lookout!";
            return progress.Has(BackyardClue.Ribbon) ? ordinary : "Rio: My blanket ribbon went missing outside. Keeterman knows the scent trail. Do bring it back neatly.";
        }
    }
}
