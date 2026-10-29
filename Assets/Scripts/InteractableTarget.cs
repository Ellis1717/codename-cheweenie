using UnityEngine;

namespace CodenameCheweenie
{
    public sealed class InteractableTarget : MonoBehaviour
    {
        private static readonly System.Collections.Generic.List<InteractableTarget> Targets = new System.Collections.Generic.List<InteractableTarget>();
        public static System.Collections.Generic.IReadOnlyList<InteractableTarget> Active => Targets;
        [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.SubsystemRegistration)]
        private static void ResetTargets() { Targets.Clear(); }
        private void OnEnable() { if (!Targets.Contains(this)) Targets.Add(this); }
        private void OnDisable() { Targets.Remove(this); }
        public int stage;
        public BackyardClue backyardClue;
        public NeighbourhoodStop neighbourhoodStop;
        [TextArea] public string dialogue;
        public bool CanReach(Transform player, float range)
        {
            if (!player || Vector3.Distance(player.position, transform.position) > range) return false;
            if (Physics.Linecast(player.position + Vector3.up * .1f, transform.position, out RaycastHit hit, ~(1 << 2), QueryTriggerInteraction.Ignore))
                return hit.transform == transform || hit.transform.IsChildOf(transform);
            return true;
        }
    }
}
