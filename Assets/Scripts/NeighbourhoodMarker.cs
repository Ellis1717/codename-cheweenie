using UnityEngine;

namespace CodenameCheweenie
{
    public sealed class NeighbourhoodMarker : MonoBehaviour
    {
        public NeighbourhoodStop stop;
        private NeighbourhoodDirector director;
        private Renderer view;
        private int state = -1;
        private void Start() { director = GetComponentInParent<NeighbourhoodDirector>(); view = GetComponent<Renderer>(); }
        private void LateUpdate()
        {
            if (!director || !view || state == director.State) return;
            state = director.State;
            view.sharedMaterial = WorldMaterials.For(director.Has(stop) ? new Color(.35f, .7f, .49f) : BrandTheme.Amber);
        }
    }
}
