using System;
using UnityEngine;
namespace CodenameCheweenie
{
    [Serializable] public sealed class SaveData
    {
        public int version = 1;
        public int stage;
        public int lakituFriendship, rioFriendship;
        public bool lakituQuest, rioQuest;
        public float volume = .6f, cameraScale = 1;
        public bool particles = true;
        public int[] keyboard, gamepad;
    }
    public static class GameSave
    {
        private const string Key = "Cheweenie.Save.v1";
        public static bool PersistenceEnabled { get; set; } = true;
        public static SaveData Data { get; private set; }
        [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.SubsystemRegistration)]
        private static void ResetStatics() { Data = null; PersistenceEnabled = true; }
        public static void Reload() { Data = null; Load(); }
        public static void Load()
        {
            if (Data != null) return;
            try { Data = JsonUtility.FromJson<SaveData>(PlayerPrefs.GetString(Key, "")); }
            catch (ArgumentException) { Data = null; }
            if (Data == null || Data.version != 1) Data = new SaveData();
            Data.stage = Mathf.Clamp(Data.stage, 0, QuestProgress.CompletedStage);
            Data.volume = float.IsNaN(Data.volume) ? .6f : Mathf.Clamp01(Data.volume);
            Data.cameraScale = float.IsNaN(Data.cameraScale) ? 1 : Mathf.Clamp(Data.cameraScale, .65f, 1.4f);
        }
        public static void Capture()
        {
            Load();
            var quest = UnityEngine.Object.FindFirstObjectByType<QuestDirector>();
            if (quest) Data.stage = quest.CurrentStage;
            foreach (var cat in UnityEngine.Object.FindObjectsByType<CatCompanion>(FindObjectsSortMode.None))
            {
                if (cat.energetic) { Data.lakituFriendship = cat.Friendship; Data.lakituQuest = cat.CompanionQuestComplete; }
                else { Data.rioFriendship = cat.Friendship; Data.rioQuest = cat.CompanionQuestComplete; }
            }
            if (PersistenceEnabled) { PlayerPrefs.SetString(Key, JsonUtility.ToJson(Data)); PlayerPrefs.Save(); }
        }
        public static void NewGame()
        {
            Load(); Data.stage = 0; Data.lakituFriendship = Data.rioFriendship = 0;
            Data.lakituQuest = Data.rioQuest = false;
            var quest = UnityEngine.Object.FindFirstObjectByType<QuestDirector>();
            if (quest) quest.Restore(0);
            foreach (var cat in UnityEngine.Object.FindObjectsByType<CatCompanion>(FindObjectsSortMode.None)) cat.Restore(0, false);
            Capture();
        }
    }
}
