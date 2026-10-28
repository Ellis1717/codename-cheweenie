using UnityEngine;

namespace CodenameCheweenie
{
    public sealed class InteractableTarget : MonoBehaviour
    {
        public int stage;
        public BackyardClue backyardClue;
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
