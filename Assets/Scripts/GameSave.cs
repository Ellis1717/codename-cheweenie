using System;
using UnityEngine;
namespace CodenameCheweenie
{
    [Serializable] public sealed class SaveData
    {
        public int version = 1;
        public int stage;
        public int backyardState;
        public int neighbourhoodState;
        public int lakituFriendship, rioFriendship;
        public bool lakituQuest, rioQuest;
        public float volume = .6f, cameraScale = 1;
        public int audioVersion;
        public float musicVolume;
        public bool particles = true;
        public bool playStationLabels;
        public int[] keyboard, gamepad;
    }
    public static class GameSave
    {
        private const string Key = "Cheweenie.Save.v1";
        private static string ReadKey => GameSelfTest.Requested ? Key + ".SelfTest" : Key;
        public static bool PersistenceEnabled { get; set; } = true;
        public static SaveData Data { get; private set; }
        [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.SubsystemRegistration)]
        private static void ResetStatics() { Data = null; PersistenceEnabled = true; }
        public static void Reload() { Data = null; Load(); }
        public static void Load()
        {
            if (Data != null) return;
            try { Data = JsonUtility.FromJson<SaveData>(PlayerPrefs.GetString(ReadKey, "")); }
            catch (ArgumentException) { Data = null; }
            if (Data == null || Data.version != 1) Data = new SaveData();
            Data.stage = Mathf.Clamp(Data.stage, 0, QuestProgress.CompletedStage);
            Data.backyardState = Data.stage == QuestProgress.CompletedStage ? new BackyardProgress(Data.backyardState).State : 0;
            Data.neighbourhoodState = new NeighbourhoodProgress(Data.neighbourhoodState).State;
            Data.volume = AudioPreferences.Unit(Data.volume, .6f);
            Data.musicVolume = AudioPreferences.RestoreMusic(Data.audioVersion, Data.musicVolume, Data.volume);
            Data.audioVersion = 1;
            Data.cameraScale = float.IsNaN(Data.cameraScale) ? 1 : Mathf.Clamp(Data.cameraScale, .65f, 1.4f);
        }
        public static void Capture()
        {
            Load();
            var quest = UnityEngine.Object.FindFirstObjectByType<QuestDirector>();
            if (quest) Data.stage = quest.CurrentStage;
            var backyard = UnityEngine.Object.FindFirstObjectByType<BackyardDirector>();
            if (backyard) Data.backyardState = backyard.State;
            var neighbourhood = UnityEngine.Object.FindFirstObjectByType<NeighbourhoodDirector>();
            if (neighbourhood) Data.neighbourhoodState = neighbourhood.State;
            foreach (var cat in UnityEngine.Object.FindObjectsByType<CatCompanion>(FindObjectsSortMode.None))
            {
                if (cat.energetic) { Data.lakituFriendship = cat.Friendship; Data.lakituQuest = cat.CompanionQuestComplete; }
                else { Data.rioFriendship = cat.Friendship; Data.rioQuest = cat.CompanionQuestComplete; }
            }
            if (PersistenceEnabled) { PlayerPrefs.SetString(Key, JsonUtility.ToJson(Data)); PlayerPrefs.Save(); }
        }
        public static void NewGame()
        {
            Load(); Data.stage = 0; Data.backyardState = 0; Data.lakituFriendship = Data.rioFriendship = 0;
            Data.neighbourhoodState = 0;
            Data.lakituQuest = Data.rioQuest = false;
            var quest = UnityEngine.Object.FindFirstObjectByType<QuestDirector>();
            if (quest) quest.Restore(0);
            UnityEngine.Object.FindFirstObjectByType<BackyardDirector>()?.Restore(0);
            UnityEngine.Object.FindFirstObjectByType<NeighbourhoodDirector>()?.Restore(0);
            foreach (var cat in UnityEngine.Object.FindObjectsByType<CatCompanion>(FindObjectsSortMode.None)) cat.ResetToHome();
            UnityEngine.Object.FindFirstObjectByType<MartyController>()?.ResetToHome();
            Capture();
        }
    }
}
