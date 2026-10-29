using UnityEngine;

namespace CodenameCheweenie
{
    public sealed class CheweenieCamera : MonoBehaviour
    {
        public Transform target;
        public Vector3 offset = new Vector3(0, 5.5f, -8f);
        private bool positioned;
        private void LateUpdate()
        {
            if (!target) return;
            GameSave.Load();
            Vector3 destination = target.position + offset * GameSave.Data.cameraScale;
            Vector3 focus = target.position + Vector3.up * .4f;
            Vector3 delta = destination - focus;
            if (Physics.SphereCast(focus, .2f, delta.normalized, out RaycastHit hit, delta.magnitude, ~0, QueryTriggerInteraction.Ignore))
                destination = focus + delta.normalized * Mathf.Max(.5f, hit.distance - .2f);
            transform.position = positioned ? Vector3.Lerp(transform.position, destination, 7f * Time.deltaTime) : destination;
            positioned = true;
            transform.LookAt(target.position + Vector3.up * .4f);
        }
    }
}
