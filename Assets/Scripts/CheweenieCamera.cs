using UnityEngine;

namespace CodenameCheweenie
{
    public sealed class CheweenieCamera : MonoBehaviour
    {
        public Transform target;
        public Vector3 offset = new Vector3(0, 5.5f, -8f);
        private void LateUpdate()
        {
            if (!target) return;
            Vector3 destination = target.position + offset;
            transform.position = Vector3.Lerp(transform.position, destination, 7f * Time.deltaTime);
            transform.LookAt(target.position + Vector3.up * .4f);
        }
    }
}
