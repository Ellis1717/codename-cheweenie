using UnityEngine;

namespace CodenameCheweenie
{
    public sealed class FrontEndMenu : MonoBehaviour
    {
        public bool ShowingTitle { get; private set; }
        private MartyInput controls;
        private CozySettings settings;
        private BrandTheme theme;
        private int selected, page, transitionFrame;
        private bool confirmNewGame, stickReleased = true;
        private float playTimeScale = 1;
        private bool HasProgress => GameSave.Data.stage > 0 || GameSave.Data.backyardState > 0 || GameSave.Data.lakituFriendship > 0 || GameSave.Data.rioFriendship > 0;

        public void Initialize(MartyInput input, CozySettings options)
        {
            controls = input; settings = options; GameSave.Load(); ShowTitle();
        }
        public void ShowTitle()
        {
            if (!ShowingTitle) playTimeScale = Time.timeScale > 0 ? Time.timeScale : 1;
            ShowingTitle = true; controls.Blocked = true; Time.timeScale = 0;
            page = selected = 0; confirmNewGame = false; transitionFrame = Time.frameCount;
        }
        public void StartAdventure(bool newGame)
        {
            if (newGame) GameSave.NewGame();
            ShowingTitle = false; controls.Blocked = false; controls.SuppressForFrame();
            Time.timeScale = playTimeScale; transitionFrame = Time.frameCount;
        }
        public void SettingsClosed() { transitionFrame = Time.frameCount; }
        private void Update()
        {
            if (!ShowingTitle || settings.IsOpen || Time.frameCount == transitionFrame) return;
            if (Input.GetKeyDown(KeyCode.Escape))
            { if (page != 0 || confirmNewGame) { page = 0; confirmNewGame = false; } else settings.OpenFromTitle(); return; }
            float axis = Input.GetAxisRaw("DogStickY");
            if (Mathf.Abs(axis) < .3f) stickReleased = true;
            int direction = Input.GetKeyDown(KeyCode.DownArrow) ? 1 : Input.GetKeyDown(KeyCode.UpArrow) ? -1 : 0;
            if (stickReleased && Mathf.Abs(axis) > .6f) { direction = axis > 0 ? 1 : -1; stickReleased = false; }
            if (direction != 0) { selected = (selected + direction + 6) % 6; confirmNewGame = false; }
            if (Input.GetKeyDown(KeyCode.Return) || Input.GetKeyDown(KeyCode.JoystickButton0) || Input.GetKeyDown(KeyCode.JoystickButton1))
                Activate(page == 0 ? selected : -1);
        }
        private void Activate(int row)
        {
            if (Time.frameCount == transitionFrame) return;
            transitionFrame = Time.frameCount;
            if (row != 1) confirmNewGame = false;
            GetComponent<GameAudio>()?.Play(SoundCue.Menu);
            if (row < 0) { page = 0; return; }
            if (row == 0) StartAdventure(false);
            else if (row == 1)
            {
                if (HasProgress && !confirmNewGame) { confirmNewGame = true; return; }
                StartAdventure(true);
            }
            else if (row == 2) settings.OpenFromTitle();
            else if (row == 3) page = 1;
            else if (row == 4) page = 2;
            else Application.Quit();
        }
        private void OnGUI()
        {
            if (!ShowingTitle) return;
            if (theme == null) theme = new BrandTheme();
            int oldDepth = GUI.depth; GUI.depth = 10;
            Matrix4x4 old = GUI.matrix;
            float scale = Mathf.Min(Screen.width / 1280f, Screen.height / 800f);
            float width = Screen.width / scale, height = Screen.height / scale;
            GUI.matrix = Matrix4x4.Scale(Vector3.one * scale);
            theme.Background(width, height);
            if (!settings.IsOpen)
            {
                theme.Paw(64, 54, 40);
                GUI.Label(new Rect(119, 57, 380, 32), "MARTY'S COZY ADVENTURE", BrandTheme.Text(16, BrandTheme.Amber, true));
                GUI.Label(new Rect(64, 124, 490, 45), "Codename ....", BrandTheme.Text(32, BrandTheme.Cream, true));
                GUI.Label(new Rect(60, 164, 610, 95), "Cheweenie", BrandTheme.Text(76, BrandTheme.Cream, true));
                GUI.Label(new Rect(66, 268, 435, 64), "A little dog. A home full of adventures.\nWith Marty, Lakitu & Rio.", BrandTheme.Text(20, BrandTheme.Cream));
                if (page == 0)
                {
                    string[] labels = { HasProgress ? "Continue adventure" : "Start adventure", confirmNewGame ? "Confirm new adventure" : "New adventure", "Settings", "How to play", "About", "Quit" };
                    for (int i = 0; i < labels.Length; i++)
                        if (BrandTheme.Button(new Rect(68, 357 + i * 52, 386, 44), labels[i], selected == i, i == 0)) Activate(i);
                    if (confirmNewGame) GUI.Label(new Rect(68, 680, 430, 42), "This resets missions and friendship. Settings stay saved.", BrandTheme.Text(16, BrandTheme.Cream));
                }
                else
                {
                    BrandTheme.Fill(new Rect(64, 348, 520, 390), new Color(.045f, .065f, .09f, .96f));
                    string text = page == 1
                        ? "Make yourself at home\n\nWASD / left stick: move\n" + controls.BindingLabel(DogAction.Jump) + ": jump\n"
                            + controls.BindingLabel(DogAction.Sprint) + ": sprint\n" + controls.BindingLabel(DogAction.Interact) + ": sniff and greet\n"
                            + controls.BindingLabel(DogAction.Cozy) + ": Cheeween Cozy\n" + controls.BindingLabel(DogAction.Between) + ": Cheween in Between\n"
                            + controls.BindingLabel(DogAction.Menu) + ": settings\n\nGreet the cats, sniff the backyard, then get Cozy. Change buttons in Settings."
                        : "Codename .... Cheweenie\n\nStarring Marty the Chiweenie, Lakitu (Keetz), and Rio (Miss Priss).\n\nMarty's specialties: Cheeween Cozy and Cheween in Between.\n\nOriginal cover concept art. House and character models are still in development.";
                    GUI.Label(new Rect(86, 366, 474, 306), text, BrandTheme.Text(18, BrandTheme.Cream));
                    if (BrandTheme.Button(new Rect(86, 682, 474, 40), "Back", true)) Activate(-1);
                }
                GUI.Label(new Rect(68, height - 45, 900, 30), "Arrows / stick to choose · Enter / controller face button to select", BrandTheme.Text(16, BrandTheme.Cream));
            }
            GUI.matrix = old; GUI.depth = oldDepth;
        }
        private void OnDestroy()
        {
            theme?.Dispose();
            if (ShowingTitle) { Time.timeScale = playTimeScale; if (controls) controls.Blocked = false; }
        }
    }
}
