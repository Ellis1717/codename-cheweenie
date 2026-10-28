using UnityEngine;
namespace CodenameCheweenie
{
    [CreateAssetMenu(menuName = "Cheweenie/Canine Tuning")]
    public sealed class CanineTuning : ScriptableObject
    {
        [Min(.1f)] public float walkSpeed = 4.4f;
        [Min(.1f)] public float sprintSpeed = 7.2f;
        [Min(.1f)] public float acceleration = 24f;
        [Min(.1f)] public float turnSpeed = 10f;
        [Min(.1f)] public float jumpSpeed = 5.5f;
        [Min(1)] public float interactionRange = 2.8f;
    }
}
