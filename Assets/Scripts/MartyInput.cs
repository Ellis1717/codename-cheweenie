using UnityEngine;
namespace CodenameCheweenie
{
    public enum DogAction { Jump, Interact, Sprint, Cozy, Between, Menu }
    public sealed class MartyInput : MonoBehaviour
    {
        public KeyCode[] keyboard = { KeyCode.Space, KeyCode.E, KeyCode.LeftShift, KeyCode.C, KeyCode.B, KeyCode.Escape };
        public KeyCode[] gamepad = { KeyCode.JoystickButton0, KeyCode.JoystickButton2, KeyCode.JoystickButton1,
            KeyCode.JoystickButton3, KeyCode.JoystickButton4, KeyCode.JoystickButton7 };
        public bool Blocked { get; set; }
        public bool Held(DogAction action) => !Blocked && (Input.GetKey(keyboard[(int)action]) || Input.GetKey(gamepad[(int)action]));
        public bool Pressed(DogAction action) => !Blocked && (Input.GetKeyDown(keyboard[(int)action]) || Input.GetKeyDown(gamepad[(int)action]));
        public Vector3 Movement
        {
            get
            {
                if (Blocked) return Vector3.zero;
                float x = (Input.GetKey(KeyCode.D) || Input.GetKey(KeyCode.RightArrow) ? 1 : 0) - (Input.GetKey(KeyCode.A) || Input.GetKey(KeyCode.LeftArrow) ? 1 : 0);
                float z = (Input.GetKey(KeyCode.W) || Input.GetKey(KeyCode.UpArrow) ? 1 : 0) - (Input.GetKey(KeyCode.S) || Input.GetKey(KeyCode.DownArrow) ? 1 : 0);
                var stick = new Vector3(Input.GetAxisRaw("DogStickX"), 0, -Input.GetAxisRaw("DogStickY"));
                return Vector3.ClampMagnitude(new Vector3(x, 0, z) + stick, 1);
            }
        }
    }
}
