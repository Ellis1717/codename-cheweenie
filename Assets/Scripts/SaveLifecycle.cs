using UnityEngine;
namespace CodenameCheweenie
{
    public sealed class SaveLifecycle : MonoBehaviour
    {
        private void Start()
        {
            GameSave.Load();
            var quest = GetComponent<QuestDirector>(); quest.Restore(GameSave.Data.stage);
            GetComponent<BackyardDirector>()?.Restore(GameSave.Data.backyardState);
            GetComponent<NeighbourhoodDirector>()?.Restore(GameSave.Data.neighbourhoodState);
            foreach (var cat in UnityEngine.Object.FindObjectsByType<CatCompanion>(FindObjectsSortMode.None))
                cat.Restore(cat.energetic ? GameSave.Data.lakituFriendship : GameSave.Data.rioFriendship,
                    cat.energetic ? GameSave.Data.lakituQuest : GameSave.Data.rioQuest);
        }
        private void OnApplicationPause(bool paused) { if (paused) GameSave.Capture(); }
        private void OnApplicationQuit() { GameSave.Capture(); }
    }
}
