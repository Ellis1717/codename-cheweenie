using UnityEngine;

namespace CodenameCheweenie
{
    public sealed class BackyardMarker : MonoBehaviour
    {
        public BackyardClue clue;
        private BackyardDirector director;
        private Renderer marker;
        private Material material;
        private int lastState = -1;
        private bool lastUnlocked;
        private void Start()
        {
            director = GetComponentInParent<BackyardDirector>(); marker = GetComponent<Renderer>();
            if (marker) material = marker.material;
        }
        private void LateUpdate()
        {
            if (!director || !material) return;
            if (lastState == director.State && lastUnlocked == director.Unlocked) return;
            lastState = director.State; lastUnlocked = director.Unlocked;
            material.color = director.Has(clue) ? new Color(.35f, .7f, .49f) : director.Available(clue) ? BrandTheme.Amber : new Color(.4f, .43f, .52f);
        }
        private void OnDestroy() { if (material) Destroy(material); }
    }
}
