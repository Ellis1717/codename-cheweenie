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
        private readonly InputReleaseGate releaseGate = new InputReleaseGate();
        public void SuppressForFrame() { releaseGate.Suppress(Time.frameCount); }
        private bool CanRead
        {
            get
            {
                bool held = false;
                if (!Blocked && releaseGate.WaitingForRelease)
                {
                    foreach (var key in keyboard) held |= Input.GetKey(key);
                    foreach (var key in gamepad) held |= Input.GetKey(key);
                }
                return releaseGate.CanRead(Time.frameCount, Blocked, held);
            }
        }
        public static bool IsBindingAllowed(int value, bool pad)
        {
            var key = (KeyCode)value;
            if (!System.Enum.IsDefined(typeof(KeyCode), key) || key == KeyCode.None) return false;
            if (pad) return key >= KeyCode.JoystickButton0 && key <= KeyCode.JoystickButton19;
            return key < KeyCode.Mouse0; // No mouse buttons or device-specific joystick aliases.
        }
        public string BindingLabel(DogAction action)
        {
            string key = keyboard[(int)action].ToString().Replace("Left", "L ").Replace("Right", "R ");
            return key + " / " + GamepadLabel(gamepad[(int)action]);
        }
        private static readonly string[] XboxButtons = { "A", "B", "X", "Y", "LB", "RB", "View", "Menu", "Left stick", "Right stick" };
        private static readonly string[] PlayStationButtons = { "Square", "Cross", "Circle", "Triangle", "L1", "R1", "L2", "R2", "Share", "Options", "L3", "R3", "PS", "Touchpad" };
        public static string GamepadLabel(KeyCode key)
        {
            GameSave.Load();
            int button = (int)key - (int)KeyCode.JoystickButton0;
            var labels = GameSave.Data.playStationLabels ? PlayStationButtons : XboxButtons;
            return button >= 0 && button < labels.Length ? labels[button] : "Pad " + button;
        }
        public bool Held(DogAction action) => CanRead && (Input.GetKey(keyboard[(int)action]) || Input.GetKey(gamepad[(int)action]));
        public bool Pressed(DogAction action) => CanRead && (Input.GetKeyDown(keyboard[(int)action]) || Input.GetKeyDown(gamepad[(int)action]));
        public Vector3 Movement
        {
            get
            {
                if (!CanRead) return Vector3.zero;
                float x = (Input.GetKey(KeyCode.D) || Input.GetKey(KeyCode.RightArrow) ? 1 : 0) - (Input.GetKey(KeyCode.A) || Input.GetKey(KeyCode.LeftArrow) ? 1 : 0);
                float z = (Input.GetKey(KeyCode.W) || Input.GetKey(KeyCode.UpArrow) ? 1 : 0) - (Input.GetKey(KeyCode.S) || Input.GetKey(KeyCode.DownArrow) ? 1 : 0);
                var stick = new Vector3(Input.GetAxisRaw("DogStickX"), 0, -Input.GetAxisRaw("DogStickY"));
                return Vector3.ClampMagnitude(new Vector3(x, 0, z) + stick, 1);
            }
        }
    }
}
