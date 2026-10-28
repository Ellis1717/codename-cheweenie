using System;
using UnityEngine;
namespace CodenameCheweenie
{
    public sealed class CozySettings : MonoBehaviour
    {
        public bool IsOpen { get; private set; }
        private MartyInput controls;
        private int selected, capture = -1;
        private bool padBindings, confirmReset;
        private float captureAfter, nextNavigate;
        private bool stickReleased = true;
        private float previousTimeScale = 1;
        private string help = "";
        private static readonly KeyCode[] DefaultKeys = { KeyCode.Space, KeyCode.E, KeyCode.LeftShift, KeyCode.C, KeyCode.B, KeyCode.Escape };
        private static readonly KeyCode[] DefaultPad = { KeyCode.JoystickButton0, KeyCode.JoystickButton2, KeyCode.JoystickButton1, KeyCode.JoystickButton3, KeyCode.JoystickButton4, KeyCode.JoystickButton7 };
        private void Start()
        {
            controls = UnityEngine.Object.FindFirstObjectByType<MartyInput>(); GameSave.Load();
            RestoreBindings(GameSave.Data.keyboard, controls.keyboard, false);
            RestoreBindings(GameSave.Data.gamepad, controls.gamepad, true);
        }
        private static void RestoreBindings(int[] saved, KeyCode[] destination, bool pad)
        {
            if (saved == null || saved.Length != destination.Length) return;
            for (int i = 0; i < saved.Length; i++)
            {
                var key = (KeyCode)saved[i];
                if (Enum.IsDefined(typeof(KeyCode), key) && IsPad(key) == pad && key != KeyCode.None) destination[i] = key;
            }
        }
        private static bool IsPad(KeyCode key) => key >= KeyCode.JoystickButton0 && key <= KeyCode.JoystickButton19;
        private void Update()
        {
            if (!controls) return;
            bool escape = Input.GetKeyDown(KeyCode.Escape);
            if (capture >= 0)
            {
                if (escape) { capture = -1; help = "Binding cancelled."; return; }
                if (Time.unscaledTime < captureAfter) return;
                foreach (KeyCode key in Enum.GetValues(typeof(KeyCode)))
                {
                    if (key == KeyCode.None || IsPad(key) != padBindings || !Input.GetKeyDown(key)) continue;
                    // Ignore per-device joystick aliases and mouse buttons when assigning keyboard actions.
                    if (!padBindings && (key >= KeyCode.JoystickButton0 || key >= KeyCode.Mouse0 && key <= KeyCode.Mouse6)) continue;
                    var bindings = padBindings ? controls.gamepad : controls.keyboard;
                    int duplicate = Array.IndexOf(bindings, key);
                    if (duplicate >= 0) bindings[duplicate] = bindings[capture];
                    bindings[capture] = key; capture = -1; Save(); help = "Binding saved. Conflicting actions are swapped."; break;
                }
                return;
            }
            bool toggle = escape || Input.GetKeyDown(controls.keyboard[(int)DogAction.Menu]) || Input.GetKeyDown(controls.gamepad[(int)DogAction.Menu]);
            if (toggle) { SetOpen(!IsOpen); return; }
            if (!IsOpen) return;
            float axis = Input.GetAxisRaw("DogStickY");
            if (Mathf.Abs(axis) < .3f) stickReleased = true;
            int direction = Input.GetKeyDown(KeyCode.DownArrow) ? 1 : Input.GetKeyDown(KeyCode.UpArrow) ? -1 : 0;
            if (stickReleased && Mathf.Abs(axis) > .6f && Time.unscaledTime > nextNavigate)
            { direction = axis > 0 ? 1 : -1; stickReleased = false; nextNavigate = Time.unscaledTime + .2f; }
            if (direction != 0) { selected = (selected + direction + 14) % 14; confirmReset = false; }
            if (Input.GetKeyDown(KeyCode.Return) || Input.GetKeyDown(KeyCode.JoystickButton0)) Activate(selected);
        }
        private void SetOpen(bool open)
        {
            IsOpen = open; controls.Blocked = open; capture = -1; confirmReset = false;
            if (open) { previousTimeScale = Time.timeScale; Time.timeScale = 0; }
            else { Time.timeScale = previousTimeScale; Save(); }
        }
        private void Save()
        {
            GameSave.Data.keyboard = Array.ConvertAll(controls.keyboard, x => (int)x);
            GameSave.Data.gamepad = Array.ConvertAll(controls.gamepad, x => (int)x);
            GameSave.Capture();
        }
        private void Activate(int row)
        {
            var data = GameSave.Data;
            if (row == 0) data.volume = data.volume >= .99f ? 0 : Mathf.Min(1, data.volume + .2f);
            else if (row == 1) data.cameraScale = data.cameraScale > 1.25f ? .7f : data.cameraScale + .2f;
            else if (row == 2) data.particles = !data.particles;
            else if (row == 3)
            {
                // Legacy drivers differ; presets are starting points, individual actions remain remappable.
                bool xbox = controls.gamepad[0] == KeyCode.JoystickButton0;
                controls.gamepad = xbox
                    ? new[] { KeyCode.JoystickButton1, KeyCode.JoystickButton0, KeyCode.JoystickButton2, KeyCode.JoystickButton3, KeyCode.JoystickButton4, KeyCode.JoystickButton9 }
                    : (KeyCode[])DefaultPad.Clone();
            }
            else if (row == 4) padBindings = !padBindings;
            else if (row <= 10) { capture = row - 5; captureAfter = Time.unscaledTime + .25f; help = "Press a " + (padBindings ? "controller button" : "keyboard key") + ". Escape cancels."; return; }
            else if (row == 11) { controls.keyboard = (KeyCode[])DefaultKeys.Clone(); controls.gamepad = (KeyCode[])DefaultPad.Clone(); }
            else if (row == 12) { SetOpen(false); return; }
            else if (row == 13)
            {
                if (!confirmReset) { confirmReset = true; help = "Select New Game again to reset quest and friendship progress."; return; }
                GameSave.NewGame(); confirmReset = false; help = "New game started. Settings preserved.";
            }
            Save();
        }
        private string Label(int row)
        {
            var d = GameSave.Data;
            if (row == 0) return "Chime volume: " + Mathf.RoundToInt(d.volume * 100) + "%";
            if (row == 1) return "Camera distance: " + d.cameraScale.ToString("0.0") + "x";
            if (row == 2) return "Cozy particles: " + (d.particles ? "On" : "Off");
            if (row == 3) return "Controller preset: " + (controls.gamepad[0] == KeyCode.JoystickButton0 ? "Xbox / standard" : "PlayStation / custom") + " (toggle)";
            if (row == 4) return "Rebind device: " + (padBindings ? "Controller" : "Keyboard");
            if (row <= 10) return ((DogAction)(row - 5)) + ": " + (padBindings ? controls.gamepad[row - 5] : controls.keyboard[row - 5]);
            if (row == 11) return "Reset bindings";
            if (row == 12) return "Save and resume";
            return confirmReset ? "Confirm New Game" : "New Game";
        }
        private void OnGUI()
        {
            if (!IsOpen || !controls) return;
            float scale = Mathf.Min(Screen.width / 680f, Screen.height / 700f);
            Matrix4x4 old = GUI.matrix; GUI.matrix = Matrix4x4.TRS(Vector3.zero, Quaternion.identity, Vector3.one * scale);
            GUI.Box(new Rect(10, 10, 660, 680), "Cozy settings | Stick/Arrows select | A/Enter activate | Escape/Start resume");
            for (int i = 0; i < 14; i++)
                if (GUI.Button(new Rect(30, 48 + i * 39, 620, 35), (i == selected ? "> " : "") + Label(i)) && capture < 0)
                { selected = i; Activate(i); }
            GUI.Label(new Rect(30, 605, 620, 70), help);
            GUI.matrix = old;
        }
        private void OnDestroy() { if (IsOpen) { Time.timeScale = previousTimeScale; if (controls) controls.Blocked = false; } }
    }
}
