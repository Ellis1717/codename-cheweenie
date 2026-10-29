using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Controls;

namespace CodenameCheweenie
{
    // Existing KeyCode saves remain valid; named gamepad controls remove driver-specific button ordering.
    public static class GameControls
    {
        private static readonly Dictionary<KeyCode, Key> Keys = MakeKeys();
        public static bool SupportsKeyboard(KeyCode code) => Keys.ContainsKey(code);
        private static int pointerFrame = -1;
        [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.SubsystemRegistration)]
        private static void ResetPointer() { pointerFrame = -1; }
        private static Dictionary<KeyCode, Key> MakeKeys()
        {
            var result = new Dictionary<KeyCode, Key>();
            foreach (KeyCode code in Enum.GetValues(typeof(KeyCode)))
            {
                string name = code.ToString();
                if (name.StartsWith("Alpha", StringComparison.Ordinal)) name = "Digit" + name.Substring(5);
                if (name.StartsWith("Keypad", StringComparison.Ordinal)) name = "Numpad" + name.Substring(6);
                if (name == "Return") name = "Enter";
                if (name == "LeftControl") name = "LeftCtrl";
                if (name == "RightControl") name = "RightCtrl";
                if (name == "LeftCommand" || name == "LeftWindows") name = "LeftMeta";
                if (name == "RightCommand" || name == "RightWindows") name = "RightMeta";
                if (name == "Print") name = "PrintScreen";
                if (name == "Menu") name = "ContextMenu";
                if (name == "Break") name = "Pause";
                if (name == "AltGr") name = "RightAlt";
                if (Enum.TryParse(name, true, out Key key) && key != Key.None) result[code] = key;
            }
            return result;
        }
        private static ButtonControl Button(KeyCode code)
        {
            var pad = Gamepad.current; if (pad == null) return null;
            int slot = (int)code - (int)KeyCode.JoystickButton0;
            GameSave.Load(); bool ps = GameSave.Data.playStationLabels;
            switch (slot)
            {
                case 0: return ps ? pad.buttonWest : pad.buttonSouth;
                case 1: return ps ? pad.buttonSouth : pad.buttonEast;
                case 2: return ps ? pad.buttonEast : pad.buttonWest;
                case 3: return pad.buttonNorth;
                case 4: return pad.leftShoulder;
                case 5: return pad.rightShoulder;
                case 6: return ps ? pad.leftTrigger : pad.selectButton;
                case 7: return ps ? pad.rightTrigger : pad.startButton;
                case 8: return ps ? pad.selectButton : pad.leftStickButton;
                case 9: return ps ? pad.startButton : pad.rightStickButton;
                case 10: return ps ? pad.leftStickButton : null;
                case 11: return ps ? pad.rightStickButton : null;
                default: return null;
            }
        }
        public static bool GetKey(KeyCode code)
        {
            if (code >= KeyCode.JoystickButton0 && code <= KeyCode.JoystickButton19 && Gamepad.current != null) return Button(code)?.isPressed ?? false;
            if (Keyboard.current != null && Keys.TryGetValue(code, out Key key)) return Keyboard.current[key].isPressed;
            return false;
        }
        public static bool GetKeyDown(KeyCode code)
        {
            if (code >= KeyCode.JoystickButton0 && code <= KeyCode.JoystickButton19 && Gamepad.current != null) return Button(code)?.wasPressedThisFrame ?? false;
            if (Keyboard.current != null && Keys.TryGetValue(code, out Key key)) return Keyboard.current[key].wasPressedThisFrame;
            return false;
        }
        public static float GetAxisRaw(string name)
        {
            if (Gamepad.current == null) return 0;
            Vector2 stick = Gamepad.current.leftStick.ReadValue();
            if (stick.sqrMagnitude < .04f) stick = Gamepad.current.dpad.ReadValue();
            return name == "DogStickX" ? stick.x : name == "DogStickY" ? -stick.y : 0;
        }
        public static Vector2 PointerPosition(Matrix4x4 guiMatrix)
        {
            if (Mouse.current == null) return new Vector2(float.NegativeInfinity, float.NegativeInfinity);
            Vector2 screen = Mouse.current.position.ReadValue();
            return guiMatrix.inverse.MultiplyPoint3x4(new Vector3(screen.x, Screen.height - screen.y, 0));
        }
        public static bool TryPointerClick(Rect rect, Matrix4x4 guiMatrix)
        {
            if (Mouse.current == null || !Mouse.current.leftButton.wasPressedThisFrame || pointerFrame == Time.frameCount || !rect.Contains(PointerPosition(guiMatrix))) return false;
            pointerFrame = Time.frameCount; return true;
        }
    }
}
