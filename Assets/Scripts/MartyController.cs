using UnityEngine;

namespace CodenameCheweenie
{
    [RequireComponent(typeof(Rigidbody))]
    public sealed class MartyController : MonoBehaviour
    {
        public float walkSpeed = 4.4f;
        public float sprintSpeed = 7.2f;
        public float jumpImpulse = 5.5f;
        private Rigidbody body;
        private QuestDirector quest;
        private float interactionCooldown;
        private bool jumpHeld;
        private bool interactHeld;
        private bool cozyHeld;

        public void Initialize(QuestDirector director) { quest = director; }

        private void Awake() { body = GetComponent<Rigidbody>(); }

        private void Update()
        {
            interactionCooldown -= Time.deltaTime;
            bool jump = Input.GetKey(KeyCode.Space) || Input.GetKey(KeyCode.JoystickButton0);
            if (jump && !jumpHeld && Mathf.Abs(body.linearVelocity.y) < .12f)
                body.AddForce(Vector3.up * jumpImpulse, ForceMode.Impulse);
            jumpHeld = jump;

            bool interact = Input.GetKey(KeyCode.E) || Input.GetKey(KeyCode.JoystickButton2);
            if (interact && !interactHeld && interactionCooldown <= 0f) Interact();
            interactHeld = interact;

            bool cozy = Input.GetKey(KeyCode.C) || Input.GetKey(KeyCode.JoystickButton3);
            if (cozy && !cozyHeld)
            {
                if (quest != null) quest.Announce("Cheeween Cozy! Marty burrows under a blanket.");
                if (quest != null && quest.CurrentStage == 3) Interact();
            }
            cozyHeld = cozy;
        }

        private void FixedUpdate()
        {
            float horizontal = Input.GetAxisRaw("Horizontal");
            float vertical = Input.GetAxisRaw("Vertical");
            Vector3 direction = new Vector3(horizontal, 0, vertical);
            direction = Vector3.ClampMagnitude(direction, 1f);
            bool sprint = Input.GetKey(KeyCode.LeftShift) || Input.GetKey(KeyCode.JoystickButton1);
            float speed = sprint ? sprintSpeed : walkSpeed;
            Vector3 current = body.linearVelocity;
            Vector3 desired = direction * speed;
            body.linearVelocity = new Vector3(desired.x, current.y, desired.z);
            if (direction.sqrMagnitude > .02f)
                transform.rotation = Quaternion.Slerp(transform.rotation, Quaternion.LookRotation(direction), Time.fixedDeltaTime * 10f);
        }

        private void Interact()
        {
            interactionCooldown = .3f;
            var targets = Object.FindObjectsByType<InteractableTarget>(FindObjectsSortMode.None);
            InteractableTarget nearest = null;
            float best = 2.8f;
            foreach (var candidate in targets)
            {
                float dist = Vector3.Distance(transform.position, candidate.transform.position);
                if (dist < best) { nearest = candidate; best = dist; }
            }
            if (nearest != null && quest != null) quest.Activate(nearest);
            else if (quest != null) quest.Announce("Nothing close enough to sniff!");
        }
    }
}
