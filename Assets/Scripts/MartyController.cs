using UnityEngine;

namespace CodenameCheweenie
{
    [RequireComponent(typeof(Rigidbody), typeof(MartyInput))]
    public sealed class MartyController : MonoBehaviour
    {
        public CanineTuning tuning;
        private MartyInput controls;
        public bool IsCozy { get; private set; }
        private CanineVisual visual;
        private Rigidbody body;
        private QuestDirector quest;
        private float interactionCooldown;
        private bool jumpHeld;
        private bool interactHeld;
        private bool cozyHeld;

        public void Initialize(QuestDirector director) { quest = director; }

        private void Awake()
        {
            body = GetComponent<Rigidbody>(); controls = GetComponent<MartyInput>();
            tuning = Resources.Load<CanineTuning>("MartyTuning");
            if (!tuning) tuning = ScriptableObject.CreateInstance<CanineTuning>();
            body.interpolation = RigidbodyInterpolation.Interpolate;
            body.collisionDetectionMode = CollisionDetectionMode.ContinuousDynamic;
        }
        private void Start() { visual = GetComponent<CanineVisual>(); }

        private void Update()
        {
            interactionCooldown -= Time.deltaTime;
            bool jump = controls.Held(DogAction.Jump);
            if (jump && !jumpHeld && Physics.Raycast(transform.position, Vector3.down, .88f, ~0, QueryTriggerInteraction.Ignore))
                body.linearVelocity = new Vector3(body.linearVelocity.x, tuning.jumpSpeed, body.linearVelocity.z);
            jumpHeld = jump;

            bool interact = controls.Held(DogAction.Interact);
            if (interact && !interactHeld && interactionCooldown <= 0f) Interact();
            interactHeld = interact;

            bool cozy = controls.Held(DogAction.Cozy);
            if (cozy && !cozyHeld)
            {
                Interact(InteractionKind.Cozy);
            }
            cozyHeld = cozy;
            if (controls.Movement.sqrMagnitude > .01f || jump) IsCozy = false;
            if (visual) visual.Animate(new Vector2(body.linearVelocity.x, body.linearVelocity.z).magnitude, IsCozy);
            if (transform.position.y < -10) { body.position = new Vector3(0, .95f, -1); body.linearVelocity = Vector3.zero; }
        }

        private void FixedUpdate()
        {
            Vector3 direction = controls.Movement;
            bool sprint = controls.Held(DogAction.Sprint);
            float speed = IsCozy ? 0 : sprint ? tuning.sprintSpeed : tuning.walkSpeed;
            Vector3 current = body.linearVelocity;
            Vector3 desired = Vector3.MoveTowards(new Vector3(current.x, 0, current.z), direction * speed, tuning.acceleration * Time.fixedDeltaTime);
            if (Physics.Raycast(transform.position, Vector3.down, out RaycastHit ground, .92f, ~0, QueryTriggerInteraction.Ignore)
                && ground.normal.y > .6f && current.y < 1f && direction.sqrMagnitude > .01f)
            {
                Vector3 slope = Vector3.ProjectOnPlane(direction, ground.normal).normalized * speed;
                desired = Vector3.MoveTowards(new Vector3(current.x, current.y, current.z), slope, tuning.acceleration * Time.fixedDeltaTime);
                current.y = desired.y;
            }
            body.linearVelocity = new Vector3(desired.x, current.y, desired.z);
            if (direction.sqrMagnitude > .02f)
                body.MoveRotation(Quaternion.Slerp(body.rotation, Quaternion.LookRotation(direction), Time.fixedDeltaTime * tuning.turnSpeed));
        }

        private void Interact(InteractionKind kind = InteractionKind.Sniff)
        {
            interactionCooldown = .3f;
            var targets = Object.FindObjectsByType<InteractableTarget>(FindObjectsSortMode.None);
            InteractableTarget nearest = null;
            float best = tuning.interactionRange;
            foreach (var candidate in targets)
            {
                if (kind == InteractionKind.Cozy && candidate.stage != 3) continue;
                float dist = Vector3.Distance(transform.position, candidate.transform.position);
                if (dist < best) { nearest = candidate; best = dist; }
            }
            if (nearest != null && quest != null)
            {
                IsCozy = kind == InteractionKind.Cozy;
                quest.Activate(nearest, kind);
            }
            else if (quest != null) quest.Announce("Nothing close enough to sniff!");
        }
    }
}
