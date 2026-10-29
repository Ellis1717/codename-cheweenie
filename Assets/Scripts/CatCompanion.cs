using UnityEngine;
namespace CodenameCheweenie
{
    public enum CatState { Rest, Prowl, Inspect, Follow }
    [RequireComponent(typeof(Rigidbody))]
    public sealed class CatCompanion : MonoBehaviour
    {
        public bool energetic;
        public int Friendship { get; private set; }
        public bool CompanionQuestComplete { get; private set; }
        public CatState State { get; private set; }
        public string CompanionObjective => CompanionQuestComplete ? "Companion quest complete"
            : energetic ? "Keeterman: say hello, then sniff the backyard" : "Miss Priss: say hello, then get Cozy at the fort";
        private Rigidbody body;
        private Transform player;
        private Vector3 home;
        private float nextState;
        private float nextFriendship;
        private Vector3 destination;
        private Vector3 velocity;
        private void Start()
        {
            body = GetComponent<Rigidbody>(); body.isKinematic = true;
            gameObject.layer = 2; // Ignore Raycast: probes must not hit the querying cat.
            body.interpolation = RigidbodyInterpolation.Interpolate;
            home = transform.position; player = Object.FindFirstObjectByType<MartyController>()?.transform;
            State = CatState.Rest; nextState = Time.time + (energetic ? 1 : 5);
        }
        public string Befriend()
        {
            if (Time.time >= nextFriendship)
            {
                Friendship = Mathf.Min(3, Friendship + 1); nextFriendship = Time.time + 8;
            }
            State = CatState.Inspect; nextState = Time.time + 2;
            return energetic ? "Keeterman chirps: come sniff the backyard with me!" : "Miss Priss gives a slow blink. A quiet blanket would be acceptable.";
        }
        public bool CompleteAt(int completedStage)
        {
            if (Friendship == 0 || CompanionQuestComplete || completedStage != (energetic ? 2 : 3)) return false;
            CompanionQuestComplete = true; Friendship = Mathf.Min(3, Friendship + 1); return true;
        }
        public void Restore(int friendship, bool complete)
        { Friendship = Mathf.Clamp(friendship, 0, 3); CompanionQuestComplete = complete; }
        public void ResetToHome()
        {
            Restore(0, false); nextFriendship = 0; State = CatState.Rest;
            nextState = Time.time + (energetic ? 1 : 5); destination = home;
            velocity=Vector3.zero;
            if (body) { var interpolation=body.interpolation;body.interpolation=RigidbodyInterpolation.None;body.position = home; body.rotation = Quaternion.identity;transform.SetPositionAndRotation(body.position,body.rotation);Physics.SyncTransforms();body.interpolation=interpolation; }
            GetComponentInChildren<GroundedGait>()?.ResetContacts();
        }
        private void FixedUpdate()
        {
            if (!player) return;
            if (Time.time >= nextState)
            {
                if (energetic && CompanionQuestComplete) State = CatState.Follow;
                else State = State == CatState.Rest ? CatState.Prowl : CatState.Rest;
                nextState = Time.time + (energetic ? 2.5f : 6f);
                destination = home + new Vector3(Mathf.Sin(Time.time * 1.7f), 0, Mathf.Cos(Time.time)) * (energetic ? 1.4f : .5f);
            }
            if (State == CatState.Inspect)
            {
                Vector3 look = player.position - body.position; look.y = 0;
                if (look.sqrMagnitude > .01f) body.MoveRotation(Quaternion.RotateTowards(body.rotation,Quaternion.LookRotation(look),Time.fixedDeltaTime*160));
            }
            Vector3 target = State == CatState.Follow ? player.position : destination;
            Vector3 delta = target - body.position; delta.y = 0;
            float stopping=State==CatState.Follow?2:.2f;
            float speed=State==CatState.Rest||State==CatState.Inspect?0:(energetic?1.8f:.6f)*Mathf.Clamp01((delta.magnitude-stopping)/.6f);
            if(delta.sqrMagnitude>.01f&&speed>0)
            {
                body.MoveRotation(Quaternion.RotateTowards(body.rotation,Quaternion.LookRotation(delta),Time.fixedDeltaTime*(energetic?180:110)));
                speed*=Mathf.Clamp01((Vector3.Dot(transform.forward,delta.normalized)+.2f)/1.2f);
            }
            velocity=Vector3.MoveTowards(velocity,delta.normalized*speed,Time.fixedDeltaTime*(energetic?3.5f:1.5f));
            if(velocity.sqrMagnitude<.0001f)return;
            Vector3 step = velocity * Time.fixedDeltaTime;
            // Simple obstacle avoidance: stop instead of pushing through furniture or climbing stories.
            if (Physics.SphereCast(body.position + Vector3.up * .15f, .3f, velocity.normalized, out _, .6f, ~(1 << 2), QueryTriggerInteraction.Ignore)) {velocity=Vector3.zero;return;}
            Vector3 next = body.position + step;
            if (!Physics.Raycast(next + Vector3.up, Vector3.down, out RaycastHit floor, 2f, ~(1 << 2), QueryTriggerInteraction.Ignore)) return;
            if (Mathf.Abs(floor.point.y + .5f - home.y) > .3f) return;
            next.y = floor.point.y + .5f; body.MovePosition(next);
        }
    }
}
