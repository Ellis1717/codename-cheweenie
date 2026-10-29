using UnityEngine;

namespace CodenameCheweenie
{
    // The saved prefab contains these articulated joints; no runtime mesh generation.
    public sealed class MartyModelRig : MonoBehaviour
    {
        public Transform[] legs, knees, ears, eyes;
        public Transform tail, head;
        public string sourceModel, sourceSha256;
        public int sourceVertexCount, sourceTriangleCount;
        private Quaternion[] legRest, kneeRest, earRest;
        private Quaternion tailRest, headRest;
        private float phase;
        public GroundedGait Gait {get;private set;}
        private void Start() { CachePose(); }
        private void CachePose()
        {
            if (legRest != null) return;
            legRest = Rest(legs); kneeRest = Rest(knees); earRest = Rest(ears);
            tailRest = tail ? tail.localRotation : Quaternion.identity;
            headRest = head ? head.localRotation : Quaternion.identity;
            Gait=gameObject.AddComponent<GroundedGait>();Gait.Configure(legs,knees,"Marty");
        }
        private static Quaternion[] Rest(Transform[] joints)
        {
            if (joints == null) return new Quaternion[0];
            var result = new Quaternion[joints.Length];
            for (int i = 0; i < result.Length; i++) result[i] = joints[i].localRotation;
            return result;
        }
        public void Animate(float speed, bool cozy, bool between)
        {
            CachePose();
            Gait.Animate(cozy,between);phase=Gait.Cycle*Mathf.PI*2;
            if (tail) tail.localRotation = tailRest * Quaternion.Euler(cozy ? 15 : 0, Mathf.Sin(Time.time*2.2f)*(cozy?2:speed>.5f?7:3), 0);
            if (head) head.localRotation = Quaternion.Slerp(head.localRotation,headRest * Quaternion.Euler(cozy ? 13 : Mathf.Sin(phase * 2) * Mathf.Min(speed, 3) * .45f, between ? -12 : 0, 0),1-Mathf.Exp(-10*Time.deltaTime));
            for (int i = 0; i < earRest.Length; i++) ears[i].localRotation = earRest[i] * Quaternion.Euler(Mathf.Sin(phase + i) * Mathf.Min(speed, 5), 0, cozy ? (i == 0 ? 6 : -6) : 0);
            float blink = Mathf.Repeat(Time.time, 4.7f) < .13f ? .14f : 1;
            if (eyes != null) foreach (var eye in eyes) eye.localScale = new Vector3(1, blink, 1);
        }
    }
}
