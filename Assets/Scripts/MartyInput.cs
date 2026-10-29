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
        public int BindingSignature
        {
            get
            {
                int hash = GameSave.Data != null && GameSave.Data.playStationLabels ? 1 : 0;
                for (int i = 0; i < keyboard.Length; i++) hash = unchecked(hash * 397 ^ (int)keyboard[i] ^ (int)gamepad[i]);
                return hash;
            }
        }
        private readonly InputReleaseGate releaseGate = new InputReleaseGate();
        public void SuppressForFrame() { releaseGate.Suppress(Time.frameCount); }
        private bool CanRead
        {
            get
            {
                bool held = false;
                if (!Blocked && releaseGate.WaitingForRelease)
                {
                    foreach (var key in keyboard) held |= GameControls.GetKey(key);
                    foreach (var key in gamepad) held |= GameControls.GetKey(key);
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
        public void ApplyControllerPreset(bool playStation)
        {
            GameSave.Load(); GameSave.Data.playStationLabels = playStation;
            gamepad = playStation
                ? new[] { KeyCode.JoystickButton1, KeyCode.JoystickButton0, KeyCode.JoystickButton2, KeyCode.JoystickButton3, KeyCode.JoystickButton4, KeyCode.JoystickButton9 }
                : new[] { KeyCode.JoystickButton0, KeyCode.JoystickButton2, KeyCode.JoystickButton1, KeyCode.JoystickButton3, KeyCode.JoystickButton4, KeyCode.JoystickButton7 };
        }
        public bool Held(DogAction action) => CanRead && (GameControls.GetKey(keyboard[(int)action]) || GameControls.GetKey(gamepad[(int)action]));
        public bool Pressed(DogAction action) => CanRead && (GameControls.GetKeyDown(keyboard[(int)action]) || GameControls.GetKeyDown(gamepad[(int)action]));
        public Vector3 Movement
        {
            get
            {
                if (!CanRead) return Vector3.zero;
                float x = (GameControls.GetKey(KeyCode.D) || GameControls.GetKey(KeyCode.RightArrow) ? 1 : 0) - (GameControls.GetKey(KeyCode.A) || GameControls.GetKey(KeyCode.LeftArrow) ? 1 : 0);
                float z = (GameControls.GetKey(KeyCode.W) || GameControls.GetKey(KeyCode.UpArrow) ? 1 : 0) - (GameControls.GetKey(KeyCode.S) || GameControls.GetKey(KeyCode.DownArrow) ? 1 : 0);
                var stick = new Vector3(GameControls.GetAxisRaw("DogStickX"), 0, -GameControls.GetAxisRaw("DogStickY"));
                return Vector3.ClampMagnitude(new Vector3(x, 0, z) + stick, 1);
            }
        }
    }
}
