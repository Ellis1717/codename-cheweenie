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
        private FrontEndMenu frontEnd;
        private BrandTheme theme;
        private int transitionFrame = -1;
        private int RowCount => frontEnd && !frontEnd.ShowingTitle ? 17 : 15;
        private bool horizontalReleased = true;
        private string help = "";
        private static readonly KeyCode[] DefaultKeys = { KeyCode.Space, KeyCode.E, KeyCode.LeftShift, KeyCode.C, KeyCode.B, KeyCode.Escape };
        private static readonly KeyCode[] DefaultPad = { KeyCode.JoystickButton0, KeyCode.JoystickButton2, KeyCode.JoystickButton1, KeyCode.JoystickButton3, KeyCode.JoystickButton4, KeyCode.JoystickButton7 };
        private void Start()
        {
            controls = UnityEngine.Object.FindFirstObjectByType<MartyInput>(); GameSave.Load();
            frontEnd = GetComponent<FrontEndMenu>();
            controls.keyboard = RestoreBindings(GameSave.Data.keyboard, controls.keyboard, false);
            controls.gamepad = RestoreBindings(GameSave.Data.gamepad, controls.gamepad, true);
        }
        private static KeyCode[] RestoreBindings(int[] saved, KeyCode[] defaults, bool pad)
        {
            var map = BindingMap.Restore(Array.ConvertAll(defaults, x => (int)x), saved, key => MartyInput.IsBindingAllowed(key, pad));
            return Array.ConvertAll(map, x => (KeyCode)x);
        }
        private void Update()
        {
            if (!controls) return;
            if (Time.frameCount == transitionFrame) return;
            bool escape = GameControls.GetKeyDown(KeyCode.Escape);
            if (capture >= 0)
            {
                if (escape) { capture = -1; transitionFrame = Time.frameCount; help = "Binding cancelled."; return; }
                if (Time.unscaledTime < captureAfter) return;
                foreach (KeyCode key in Enum.GetValues(typeof(KeyCode)))
                {
                    if (!MartyInput.IsBindingAllowed((int)key, padBindings) || !GameControls.GetKeyDown(key)) continue;
                    var bindings = padBindings ? controls.gamepad : controls.keyboard;
                    var map = Array.ConvertAll(bindings, x => (int)x);
                    BindingMap.Assign(map, capture, (int)key, value => MartyInput.IsBindingAllowed(value, padBindings));
                    for (int i = 0; i < map.Length; i++) bindings[i] = (KeyCode)map[i];
                    capture = -1; transitionFrame = Time.frameCount; Save(); help = "Binding saved. Conflicting actions are swapped."; break;
                }
                return;
            }
            bool toggle = escape || GameControls.GetKeyDown(controls.keyboard[(int)DogAction.Menu]) || GameControls.GetKeyDown(controls.gamepad[(int)DogAction.Menu]);
            if (!IsOpen && frontEnd && frontEnd.ShowingTitle) return;
            if (toggle) { SetOpen(!IsOpen); return; }
            if (!IsOpen) return;
            float axis = GameControls.GetAxisRaw("DogStickY");
            if (Mathf.Abs(axis) < .3f) stickReleased = true;
            int direction = GameControls.GetKeyDown(KeyCode.DownArrow) ? 1 : GameControls.GetKeyDown(KeyCode.UpArrow) ? -1 : 0;
            if (stickReleased && Mathf.Abs(axis) > .6f && Time.unscaledTime > nextNavigate)
            { direction = axis > 0 ? 1 : -1; stickReleased = false; nextNavigate = Time.unscaledTime + .2f; }
            if (direction != 0) { selected = (selected + direction + RowCount) % RowCount; confirmReset = false; return; }
            float horizontal = GameControls.GetAxisRaw("DogStickX");
            if (Mathf.Abs(horizontal) < .3f) horizontalReleased = true;
            int adjustment = GameControls.GetKeyDown(KeyCode.RightArrow) ? 1 : GameControls.GetKeyDown(KeyCode.LeftArrow) ? -1 : 0;
            if (horizontalReleased && Mathf.Abs(horizontal) > .6f) { adjustment = horizontal > 0 ? 1 : -1; horizontalReleased = false; }
            if (adjustment != 0 && selected <= 1) { AdjustVolume(selected, adjustment, false); return; }
            if (GameControls.GetKeyDown(KeyCode.Return) || GameControls.GetKeyDown(KeyCode.JoystickButton0) || GameControls.GetKeyDown(KeyCode.JoystickButton1)) Activate(selected);
        }
        public void OpenFromTitle()
        {
            if (!controls) controls = UnityEngine.Object.FindFirstObjectByType<MartyInput>();
            if (!frontEnd) frontEnd = GetComponent<FrontEndMenu>();
            selected = 0; SetOpen(true);
        }
        public void Close() { if (IsOpen) SetOpen(false); }
        public void ReturnToTitle()
        {
            if (!frontEnd) frontEnd = GetComponent<FrontEndMenu>();
            if (!frontEnd) return;
            Close(); frontEnd.ShowTitle();
        }
        public void ReturnHome()
        {
            if (!controls || frontEnd && frontEnd.ShowingTitle) return;
            controls.GetComponent<MartyController>().ResetToHome();
            if (Camera.main) Camera.main.GetComponent<CheweenieCamera>()?.SnapToTarget();
            GetComponent<QuestDirector>().Announce("Back home, Marty! Your missions and friendships are saved.");
            Close(); GameSave.Capture();
        }
        private void SetOpen(bool open)
        {
            transitionFrame = Time.frameCount;
            IsOpen = open; controls.Blocked = open || frontEnd && frontEnd.ShowingTitle; capture = -1; confirmReset = false;
            GetComponent<GameAudio>()?.Play(SoundCue.Menu);
            if (open) { previousTimeScale = Time.timeScale; Time.timeScale = 0; }
            else { Time.timeScale = previousTimeScale; controls.SuppressForFrame(); frontEnd?.SettingsClosed(); Save(); }
        }
        private void Save()
        {
            GameSave.Data.keyboard = Array.ConvertAll(controls.keyboard, x => (int)x);
            GameSave.Data.gamepad = Array.ConvertAll(controls.gamepad, x => (int)x);
            GameSave.Capture();
        }
        private void Activate(int row)
        {
            if (Time.frameCount == transitionFrame) return;
            transitionFrame = Time.frameCount;
            if (row != 14) confirmReset = false;
            var data = GameSave.Data;
            if (row <= 1) { AdjustVolume(row, 1, true); return; }
            GetComponent<GameAudio>()?.Play(SoundCue.Menu);
            if (row == 2) data.cameraScale = data.cameraScale > 1.25f ? .7f : data.cameraScale + .2f;
            else if (row == 3) data.particles = !data.particles;
            else if (row == 4)
            {
                // Presets retain the saved button IDs while reading named Input System controls.
                controls.ApplyControllerPreset(!data.playStationLabels);
            }
            else if (row == 5) padBindings = !padBindings;
            else if (row <= 11) { capture = row - 6; captureAfter = Time.unscaledTime + .25f; help = "Press a " + (padBindings ? "controller button" : "keyboard key") + ". Escape cancels."; return; }
            else if (row == 12) { controls.keyboard = (KeyCode[])DefaultKeys.Clone(); controls.gamepad = (KeyCode[])DefaultPad.Clone(); }
            else if (row == 13) { SetOpen(false); return; }
            else if (row == 14)
            {
                if (!confirmReset) { confirmReset = true; help = "Select New Game again to reset quest and friendship progress."; return; }
                GameSave.NewGame(); confirmReset = false; help = "New game started. Settings preserved.";
            }
            else if (row == 15) { ReturnToTitle(); return; }
            else if (row == 16) { ReturnHome(); return; }
            Save();
        }
        private void AdjustVolume(int row, int direction, bool wrap)
        {
            transitionFrame = Time.frameCount; confirmReset = false;
            if (row == 0) GameSave.Data.volume = AudioPreferences.Step(GameSave.Data.volume, direction, wrap);
            else GameSave.Data.musicVolume = AudioPreferences.Step(GameSave.Data.musicVolume, direction, wrap);
            Save(); GetComponent<GameAudio>()?.Play(SoundCue.Menu);
            help = "Music and sound effects save separately. Use Left/Right or move the stick sideways to adjust.";
        }
        private string Label(int row)
        {
            var d = GameSave.Data;
            if (row == 0) return "Sound effects: " + Mathf.RoundToInt(d.volume * 100) + "%   < / >";
            if (row == 1) return "Music: " + Mathf.RoundToInt(d.musicVolume * 100) + "%   < / >";
            if (row == 2) return "Camera distance: " + d.cameraScale.ToString("0.0") + "x";
            if (row == 3) return "Cozy particles: " + (d.particles ? "On" : "Off");
            if (row == 4) return "Controller preset: " + (d.playStationLabels ? "PlayStation" : "Xbox") + " (toggle)";
            if (row == 5) return "Rebind device: " + (padBindings ? "Controller" : "Keyboard");
            if (row <= 11) return ((DogAction)(row - 6)) + ": " + (padBindings ? MartyInput.GamepadLabel(controls.gamepad[row - 6]) : controls.keyboard[row - 6].ToString());
            if (row == 12) return "Reset bindings";
            if (row == 13) return frontEnd && frontEnd.ShowingTitle ? "Save and back to title" : "Save and resume";
            if (row == 15) return "Return to title";
            if (row == 16) return "Return Marty home (keep progress)";
            return confirmReset ? "Confirm New Game" : "New Game";
        }
        private void OnGUI()
        {
            if (!IsOpen || !controls) return;
            if (theme == null) theme = new BrandTheme();
            int oldDepth = GUI.depth; GUI.depth = -10;
            float scale = Mathf.Min(Screen.width / 800f, Screen.height / 920f);
            Matrix4x4 old = GUI.matrix; GUI.matrix = Matrix4x4.TRS(Vector3.zero, Quaternion.identity, Vector3.one * scale);
            float width = Screen.width / scale, height = Screen.height / scale;
            float x = (width - 760) * .5f, y = (height - 900) * .5f;
            theme.Background(width, height);
            BrandTheme.Fill(new Rect(x, y, 760, 900), new Color(.035f, .055f, .08f, .96f));
            theme.Paw(x + 30, y + 24, 36);
            GUI.Label(new Rect(x + 85, y + 22, 640, 32), BrandTheme.Title, BrandTheme.Text(24, BrandTheme.Cream, true));
            GUI.Label(new Rect(x + 30, y + 72, 650, 48), "Make it cozy. Make it yours.", BrandTheme.Text(30, BrandTheme.Amber, true));
            for (int i = 0; i < RowCount; i++)
                if (BrandTheme.Button(new Rect(x + 30, y + 137 + i * 38, 700, 33), (i == selected ? "> " : "") + Label(i), selected == i) && capture < 0)
                { selected = i; Activate(i); }
            GUI.Label(new Rect(x + 30, y + 780, 700, 60), help, BrandTheme.Text(17, BrandTheme.Cream));
            GUI.Label(new Rect(x + 30, y + 850, 700, 40), "Arrows / stick: choose · Enter / face button: select · Escape / Start: back", BrandTheme.Text(16, BrandTheme.Cream));
            GUI.matrix = old; GUI.depth = oldDepth;
        }
        private void OnDestroy() { theme?.Dispose(); if (IsOpen) { Time.timeScale = previousTimeScale > 0 ? previousTimeScale : 1; if (controls) controls.Blocked = false; } }
    }
}
