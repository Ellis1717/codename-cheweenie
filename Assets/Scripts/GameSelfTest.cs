using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.LowLevel;

namespace CodenameCheweenie
{
    // Explicit opt-in acceptance mode for the built Windows player; normal launches do not create virtual devices.
    public sealed class GameSelfTest : MonoBehaviour
    {
        private static readonly string[] Arguments = Environment.GetCommandLineArgs();
        public static bool Requested => Array.IndexOf(Arguments, "-cheweenie-self-test") >= 0;
        [Serializable] private sealed class Result
        {
            public bool passed;
            public string version;
            public string scenario;
            public float seconds, averageFramesPerSecond;
            public int homeStage, backyardState, neighbourhoodState, materialCount;
            public string[] checks, errors;
        }
        private readonly List<string> checks = new List<string>();
        private readonly List<string> errors = new List<string>();
        private Gamepad pad;
        private Keyboard keyboard;
        private Mouse mouse;
        private MartyController dog;
        private MartyInput controls;
        private QuestDirector quest;
        private BackyardDirector backyard;
        private string report;
        private float started;
        private int firstFrame;
        private string portraitLabel;
        private void OnGUI()
        {
            if (string.IsNullOrEmpty(portraitLabel)) return;
            BrandTheme.Fill(new Rect(28, 24, 470, 62), new Color(.035f, .055f, .08f, .95f));
            GUI.Label(new Rect(42, 34, 440, 46), portraitLabel, BrandTheme.Text(26, BrandTheme.Cream, true));
        }
        private void Check(bool condition, string description)
        {
            if (!condition) throw new InvalidOperationException(description);
            checks.Add(description); Debug.Log("[Cheweenie self-test] " + description);
        }
        private IEnumerator Start()
        {
            Application.runInBackground = true; GameSave.PersistenceEnabled = false;
            InputSystem.settings.backgroundBehavior = InputSettings.BackgroundBehavior.IgnoreFocus;
            int index = Array.IndexOf(Arguments, "-cheweenie-report");
            report = Path.GetFullPath(index >= 0 && index + 1 < Arguments.Length ? Arguments[index + 1] : "self-test.json");
            Directory.CreateDirectory(Path.GetDirectoryName(report));
            Application.logMessageReceived += Log;
            started = Time.realtimeSinceStartup; firstFrame = Time.frameCount;
            var stack = new Stack<IEnumerator>(); stack.Push(Run());
            while (stack.Count > 0)
            {
                object current = null; bool moved = false; Exception failure = null;
                try { moved = stack.Peek().MoveNext(); if (moved) current = stack.Peek().Current; }
                catch (Exception exception) { failure = exception; }
                if (failure != null || Time.realtimeSinceStartup - started > 600)
                { errors.Add(failure != null ? failure.ToString() : "Self-test exceeded 600 seconds"); Finish(false); yield break; }
                if (!moved) { stack.Pop(); continue; }
                if (current is IEnumerator nested) stack.Push(nested);
                else yield return current;
            }
            Finish(errors.Count == 0);
        }
        private void Log(string message, string stack, LogType type)
        {
            if ((type == LogType.Error || type == LogType.Exception || type == LogType.Assert) && errors.Count < 100) errors.Add(message + "\n" + stack);
        }
        private void Finish(bool passed)
        {
            var result = new Result
            {
                passed = passed, version = Application.version, seconds = Time.realtimeSinceStartup - started,
                scenario = Array.IndexOf(Arguments, "-cheweenie-save-write") >= 0 ? "save-write" : Array.IndexOf(Arguments, "-cheweenie-save-read") >= 0 ? "save-read" : Array.IndexOf(Arguments, "-cheweenie-menu-smoke") >= 0 ? "menus-and-visuals" : Array.IndexOf(Arguments, "-cheweenie-family-smoke") >= 0 ? "family-cameos" : "full-adventure",
                averageFramesPerSecond = (Time.frameCount - firstFrame) / Mathf.Max(.01f, Time.realtimeSinceStartup - started),
                homeStage = quest ? quest.CurrentStage : -1, backyardState = backyard ? backyard.State : -1,
                neighbourhoodState = GetComponent<NeighbourhoodDirector>() ? GetComponent<NeighbourhoodDirector>().State : -1,
                materialCount = Resources.FindObjectsOfTypeAll<Material>().Length, checks = checks.ToArray(), errors = errors.ToArray()
            };
            File.WriteAllText(report, JsonUtility.ToJson(result, true));
            Application.logMessageReceived -= Log;
            if (pad != null) InputSystem.RemoveDevice(pad);
            if (keyboard != null) InputSystem.RemoveDevice(keyboard);
            if (mouse != null) InputSystem.RemoveDevice(mouse);
            Application.Quit(passed ? 0 : 2);
        }
        private void Pad(Vector2 movement, GamepadButton? button = null)
        {
            var state = new GamepadState { leftStick = movement };
            if (button.HasValue) state = state.WithButton(button.Value);
            InputSystem.QueueStateEvent(pad, state);
        }
        private IEnumerator Shot(string name)
        {
            yield return new WaitForEndOfFrame();
            ScreenCapture.CaptureScreenshot(Path.Combine(Path.GetDirectoryName(report), name + ".png"));
            yield return new WaitForSecondsRealtime(.2f);
        }
        private IEnumerator ClickGui(Vector2 point, float scale)
        {
            Vector2 screen = new Vector2(point.x * scale, Screen.height - point.y * scale);
            InputSystem.QueueStateEvent(mouse, new MouseState { position = screen }.WithButton(MouseButton.Left));
            yield return new WaitForSecondsRealtime(.12f);
            InputSystem.QueueStateEvent(mouse, new MouseState { position = screen });
            yield return new WaitForSecondsRealtime(.12f);
        }
        private IEnumerator Drive(float x, float z, bool sprint = false)
        {
            float until = Time.realtimeSinceStartup + Mathf.Max(15, Vector3.Distance(dog.transform.position, new Vector3(x, dog.transform.position.y, z)) / 3 + 5);
            float avoidUntil = 0;
            Vector2 avoid = Vector2.zero;
            while (Vector2.Distance(new Vector2(dog.transform.position.x, dog.transform.position.z), new Vector2(x, z)) > .4f)
            {
                if (Time.realtimeSinceStartup > until) throw new InvalidOperationException("Traversal blocked toward " + x + ", " + z + " at " + dog.transform.position);
                Vector2 direction = new Vector2(x - dog.transform.position.x, z - dog.transform.position.z);
                if (Time.realtimeSinceStartup < avoidUntil) direction = avoid;
                else if (Physics.SphereCast(dog.transform.position, .43f, new Vector3(direction.x, 0, direction.y).normalized,
                    out RaycastHit obstacle, .8f, ~0, QueryTriggerInteraction.Ignore) && obstacle.transform.GetComponent<CatCompanion>())
                {
                    // Walk around a companion who has followed into the route, just as a player would.
                    avoid = new Vector2(-direction.y, direction.x).normalized;
                    avoidUntil = Time.realtimeSinceStartup + .7f; direction = avoid;
                }
                Pad(Vector2.ClampMagnitude(direction * 1.5f, 1), sprint ? GamepadButton.East : (GamepadButton?)null); yield return null;
            }
            Pad(Vector2.zero); yield return new WaitForSecondsRealtime(.15f);
        }
        private InteractableTarget Target(int stage, BackyardClue clue = BackyardClue.None)
        {
            foreach (var target in InteractableTarget.Active) if (target.stage == stage && target.backyardClue == clue && target.neighbourhoodStop == NeighbourhoodStop.None) return target;
            throw new InvalidOperationException("Missing interaction " + stage + " / " + clue);
        }
        private IEnumerator Interact(InteractableTarget target, bool cozy = false)
        {
            float until = Time.realtimeSinceStartup + 12;
            while (!target.CanReach(dog.transform, 2.3f))
            {
                if (Time.realtimeSinceStartup > until) throw new InvalidOperationException("Cannot approach " + target.name + " from " + dog.transform.position);
                Vector3 delta = target.transform.position - dog.transform.position;
                Pad(new Vector2(delta.x, delta.z).normalized); yield return null;
            }
            Pad(Vector2.zero); yield return new WaitForSecondsRealtime(.12f);
            Pad(Vector2.zero, cozy ? GamepadButton.North : GamepadButton.West); yield return new WaitForSecondsRealtime(.12f);
            Pad(Vector2.zero); yield return new WaitForSecondsRealtime(.2f);
        }
        private InteractableTarget Stop(NeighbourhoodStop stop)
        {
            foreach (var target in InteractableTarget.Active) if (target.neighbourhoodStop == stop) return target;
            throw new InvalidOperationException("Missing neighbourhood stop " + stop);
        }
        private IEnumerator Run()
        {
            pad = InputSystem.AddDevice<Gamepad>(); keyboard = InputSystem.AddDevice<Keyboard>(); mouse = InputSystem.AddDevice<Mouse>();
            dog = FindFirstObjectByType<MartyController>(); controls = dog.GetComponent<MartyInput>();
            quest = GetComponent<QuestDirector>(); backyard = GetComponent<BackyardDirector>();
            var frontEnd = GetComponent<FrontEndMenu>(); var settings = GetComponent<CozySettings>();
            yield return new WaitForSecondsRealtime(3.5f);
            while (!UnityEngine.Rendering.SplashScreen.isFinished) yield return null;
            if (Array.IndexOf(Arguments, "-cheweenie-save-write") >= 0) { yield return SaveProbe(true); yield break; }
            if (Array.IndexOf(Arguments, "-cheweenie-save-read") >= 0) { yield return SaveProbe(false); yield break; }
            Check(frontEnd.ShowingTitle && Time.timeScale == 0, "Branded title pauses gameplay");
            Check(System.Array.FindAll(FindObjectsByType<Camera>(FindObjectsSortMode.None), camera => camera.enabled).Length == 1
                && System.Array.FindAll(FindObjectsByType<AudioListener>(FindObjectsSortMode.None), listener => listener.enabled).Length == 1, "Exactly one gameplay camera and audio listener are active");
            Check(Resources.Load<AudioClip>("Audio/Home") != null, "Music bundled in the player");
            Check(GetComponent<WorldArt>()&&GetComponent<WorldArt>().Ready&&GetComponent<WorldArt>().PropCount>70, "Retained scenery palette, textured surfaces and reusable props are active");
            foreach(var cat in FindObjectsByType<CatVisual>(FindObjectsSortMode.None))Check(cat.UsesRetainedModel,cat.look+" saved character prefab and coat are active");
            Check(dog.GetComponent<CanineVisual>().UsesAuthoredModel && dog.GetComponentInChildren<MartyModelRig>(), "Saved Marty character prefab is loaded instead of the primitive fallback");
            yield return Shot("title");
            float titleScale = Mathf.Min(Screen.width / 1280f, Screen.height / 800f);
            yield return ClickGui(new Vector2(261, 483), titleScale);
            Check(settings.IsOpen && Time.timeScale == 0, "Input System mouse opens settings from title"); yield return Shot("settings");
            float settingsScale = Mathf.Min(Screen.width / 800f, Screen.height / 920f);
            float rowX = Screen.width / settingsScale * .5f, rowY = (Screen.height / settingsScale - 900) * .5f;
            float previousMusic = GameSave.Data.musicVolume;
            yield return ClickGui(new Vector2(rowX, rowY + 191.5f), settingsScale);
            Check(Mathf.Abs(previousMusic - GameSave.Data.musicVolume) > .01f, "Input System mouse adjusts the scaled settings volume row");
            yield return ClickGui(new Vector2(rowX, rowY + 647.5f), settingsScale);
            Check(!settings.IsOpen && frontEnd.ShowingTitle && Time.timeScale == 0, "Mouse closes settings without unpausing the title");
            yield return ClickGui(new Vector2(261, 379), titleScale);
            Check(!frontEnd.ShowingTitle && Time.timeScale > 0, "Input System mouse starts the adventure");
            GameSave.NewGame(); controls.ApplyControllerPreset(false);
            yield return new WaitForSecondsRealtime(.2f);
            float initial = dog.transform.position.z;
            InputSystem.QueueStateEvent(keyboard, new KeyboardState(Key.W)); yield return new WaitForSecondsRealtime(.4f);
            InputSystem.QueueStateEvent(keyboard, new KeyboardState()); yield return new WaitForSecondsRealtime(.15f);
            Check(dog.transform.position.z > initial + .6f, "Keyboard movement advances Marty");
            float ground = dog.transform.position.y;
            InputSystem.QueueStateEvent(keyboard, new KeyboardState(Key.Space)); yield return new WaitForSecondsRealtime(.2f);
            InputSystem.QueueStateEvent(keyboard, new KeyboardState());
            Check(dog.transform.position.y > ground + .3f, "Keyboard jump leaves the ground");
            yield return new WaitForSecondsRealtime(1);
            controls.ApplyControllerPreset(true);
            Pad(Vector2.zero, GamepadButton.South); yield return new WaitForSecondsRealtime(.1f);
            Check(controls.Held(DogAction.Jump), "PlayStation Cross maps to jump");
            Pad(Vector2.zero); yield return new WaitForSecondsRealtime(1);
            controls.ApplyControllerPreset(false);
            yield return CameraControllerProbe();
            if (Array.IndexOf(Arguments, "-cheweenie-menu-smoke") >= 0)
            {
                yield return MartyPortrait();
                yield return CatPortraits();
                yield return SceneryPortraits();
                yield return MappedMenu();
                settings.OpenFromTitle(); settings.ReturnToTitle();
                Check(frontEnd.ShowingTitle && Time.timeScale == 0, "Menu smoke returns to the paused title");
                Check(errors.Count == 0, "No runtime errors during the menu smoke"); yield break;
            }
            yield return Drive(0, -7.8f); yield return Drive(-6.3f, -7.8f); yield return Drive(-6.3f, 4);
            Check(dog.transform.position.y > 4.2f, "Walked up the headquarters ramp to the second story"); yield return Shot("loft");
            yield return Drive(-6.3f, -7.8f); yield return Drive(0, -7.8f);
            yield return Drive(-4.8f, -1); yield return Drive(-4.8f, 2.5f);
            yield return Interact(Target(0)); Check(quest.CurrentStage == 1, "Greeted Lakitu using gamepad interaction");
            yield return Drive(-4.8f, -1); yield return Drive(0, -1); yield return Drive(0, 2.6f); yield return Drive(5, 2.6f);
            yield return Interact(Target(1)); Check(quest.CurrentStage == 2, "Greeted Rio");
            yield return Drive(0, 2.6f); yield return Drive(0, -8);
            yield return Interact(Target(2)); Check(quest.CurrentStage == 3, "Initial backyard sniff complete");
            Check(quest.CurrentNotice.Contains("Fox Ellis"), "Fox Ellis sniff-inspector punchline survives companion quest completion");
            yield return Drive(0, 2.6f); yield return Drive(0, 5.6f);
            yield return Interact(Target(3), true); Check(quest.CurrentStage == 4 && dog.IsCozy, "Cheeween Cozy completes the home mission");
            Check(quest.CurrentNotice.Contains("Leo Ellis"), "Leo Ellis blanket-boss punchline survives companion quest completion");
            yield return Shot("cozy");
            if(Array.IndexOf(Arguments,"-cheweenie-family-smoke")>=0)
            {
                InputSystem.QueueStateEvent(keyboard,new KeyboardState(Key.C));yield return new WaitForSecondsRealtime(.15f);InputSystem.QueueStateEvent(keyboard,new KeyboardState());yield return new WaitForSecondsRealtime(.15f);
                Check(quest.CurrentNotice.Contains("Leo Ellis"),"Keyboard Cozy replays Leo Ellis cameo after completion");
                yield return Drive(0,-8);
                InputSystem.QueueStateEvent(keyboard,new KeyboardState(Key.E));yield return new WaitForSecondsRealtime(.15f);InputSystem.QueueStateEvent(keyboard,new KeyboardState());yield return new WaitForSecondsRealtime(.15f);
                Check(quest.CurrentNotice.Contains("Fox Ellis"),"Keyboard sniff replays Fox Ellis cameo after completion");
                Check(errors.Count==0,"No runtime errors during family cameo verification");yield break;
            }
            yield return Drive(3.5f, 5.3f);
            Pad(Vector2.zero, GamepadButton.LeftShoulder); yield return new WaitForSecondsRealtime(.12f);
            Pad(Vector2.zero); yield return new WaitForSecondsRealtime(1.8f);
            Check(!dog.IsBetween && Vector3.Distance(dog.transform.position, new Vector3(3.5f, .95f, 7.4f)) < .25f,
                "Cheween in Between reaches the cuddle gap using the shoulder button");
            yield return Drive(3.5f, 5.3f); yield return Drive(0, 5.3f);
            yield return Drive(0, -8); yield return Drive(-4, -9);
            yield return Interact(Target(-1, BackyardClue.ScentTrail)); Check(backyard.Has(BackyardClue.ScentTrail), "Scent trail discovered");
            yield return Drive(-10, -8.5f); yield return Drive(-10, -14);
            Check(dog.transform.position.y > 1.8f, "Walked up the lookout ramp without teleporting");
            yield return Interact(Target(-1, BackyardClue.Lookout)); yield return Shot("lookout");
            yield return Drive(-10, -8.5f); yield return Drive(11, -8.5f); yield return Drive(11, -11.6f);
            yield return Interact(Target(-1, BackyardClue.PawPassage)); Check(backyard.Has(BackyardClue.PawPassage), "Traversed the low hedge passage");
            yield return Drive(11, -18); yield return Drive(14.5f, -18);
            yield return Interact(Target(-1, BackyardClue.Ribbon)); Check(backyard.Has(BackyardClue.Ribbon), "Ribbon recovered after all route clues");
            // Retrace the clear passage; the cushion/hedge corner has insufficient
            // clearance for a tolerance-based turn at x=13.35.
            yield return Drive(11, -18); yield return Drive(11, -8.3f); yield return Drive(0, -8.3f);
            yield return Drive(0, 2.6f); yield return Drive(5, 2.6f); yield return Interact(Target(1));
            Check(backyard.Complete, "Delivered ribbon to Rio; full adventure complete"); yield return Shot("complete");
            if (MappedNeighbourhood.Current != null) yield return MappedTour();
            else
            {
            yield return Drive(0, 2.6f); yield return Drive(0, -8.3f); yield return Drive(13.35f, -8.3f);
            yield return Drive(13.35f, -13); yield return Drive(26, -13);
            Check(dog.transform.position.x > 24, "Walked through the east garden gate into the neighbourhood");
            yield return Drive(26, -19); yield return Interact(Stop(NeighbourhoodStop.Mailbox));
            Check(GetComponent<NeighbourhoodDirector>().Has(NeighbourhoodStop.Mailbox), "Mailbox stamp discovered");
            yield return Drive(26, -13); yield return Drive(47, -13); yield return Interact(Stop(NeighbourhoodStop.Garden));
            Check(GetComponent<NeighbourhoodDirector>().Has(NeighbourhoodStop.Garden), "Crossed the street and sniffed the flower garden");
            yield return Shot("neighbourhood-garden");
            yield return Drive(47, -13); yield return Drive(26, -13); yield return Drive(26, 8); yield return Drive(23, 8);
            yield return Interact(Stop(NeighbourhoodStop.Park));
            Check(GetComponent<NeighbourhoodDirector>().Has(NeighbourhoodStop.Park), "Park stamp discovered beside the pond");
            yield return Shot("neighbourhood-park");
            yield return Drive(26, 8); yield return Drive(26, -13); yield return Drive(20, -13); yield return Interact(Stop(NeighbourhoodStop.Home));
            Check(GetComponent<NeighbourhoodDirector>().Complete, "Returned to the garden gate and finished the neighbourhood patrol");
            GameSave.Capture(); Check(GameSave.Data.neighbourhoodState == 15, "Neighbourhood completion captured independently of home and backyard progress");
            yield return Drive(13.35f, -13); yield return Drive(13.35f, -8.3f); yield return Drive(0, -8.3f); yield return Drive(0, -1);
            Check(dog.transform.position.x < 2 && dog.transform.position.z > -2, "Walked home after the neighbourhood tour");
            }
            yield return RecoveryProbe();
            if(Array.IndexOf(Arguments,"-cheweenie-motion-demo")>=0)yield return MotionDemo();
            settings.OpenFromTitle(); settings.ReturnToTitle();
            Check(frontEnd.ShowingTitle && Time.timeScale == 0, "Return to title works in the built player");
            Check(errors.Count == 0, "No runtime errors during the full playthrough");
        }
        private IEnumerator RecoveryProbe()
        {
            yield return Drive(0, -7.8f); yield return Drive(13.35f, -7.8f);
            int home = quest.CurrentStage, yard = backyard.State, local = GetComponent<NeighbourhoodDirector>().State;
            float music = GameSave.Data.musicVolume, effects = GameSave.Data.volume;
            InputSystem.QueueStateEvent(keyboard, new KeyboardState(Key.Escape)); yield return new WaitForSecondsRealtime(.1f);
            InputSystem.QueueStateEvent(keyboard, new KeyboardState()); yield return new WaitForSecondsRealtime(.1f);
            var settings = GetComponent<CozySettings>(); Check(settings.IsOpen, "Keyboard opens the gameplay pause menu");
            float scale = Mathf.Min(Screen.width / 800f, Screen.height / 920f);
            yield return ClickGui(new Vector2(Screen.width / scale * .5f, (Screen.height / scale - 900) * .5f + 761.5f), scale);
            Check(!settings.IsOpen && Time.timeScale > 0 && Vector3.Distance(dog.transform.position, new Vector3(0, .95f, -1)) < .2f, "Mouse recovery returns Marty home and resumes play");
            Check(quest.CurrentStage == home && backyard.State == yard && GetComponent<NeighbourhoodDirector>().State == local
                && GameSave.Data.musicVolume == music && GameSave.Data.volume == effects, "Home recovery preserves all missions and volume settings");
            Check(Vector3.Distance(Camera.main.transform.position, dog.transform.position) < 14, "Recovery snaps the follow camera home");
        }
        private IEnumerator SaveProbe(bool write)
        {
            var neighbourhood = GetComponent<NeighbourhoodDirector>();
            int localState = MappedNeighbourhood.Current != null && MappedNeighbourhood.Current.IncludesSchool ? 31 : 15;
            if (write)
            {
                GameSave.ClearAcceptanceSave(); GameSave.PersistenceEnabled = true; GameSave.NewGame();
                quest.Restore(4); backyard.Restore(31); neighbourhood.Restore(localState);
                foreach (var cat in FindObjectsByType<CatCompanion>(FindObjectsSortMode.None)) cat.Restore(cat.energetic ? 2 : 3, true);
                controls.ApplyControllerPreset(true); controls.keyboard[(int)DogAction.Interact] = KeyCode.F;
                GameSave.Data.keyboard = System.Array.ConvertAll(controls.keyboard, key => (int)key);
                GameSave.Data.gamepad = System.Array.ConvertAll(controls.gamepad, key => (int)key);
                GameSave.Data.volume = .42f; GameSave.Data.musicVolume = .27f; GameSave.Data.cameraScale = 1.1f;GameSave.Data.cameraMode=(int)CameraPreset.Explore;
                GameSave.Capture(); Check(GameSave.Data.stage == 4 && GameSave.Data.neighbourhoodState == localState, "Isolated acceptance save written to disk");
            }
            else
            {
                Check(quest.CurrentStage == 4 && backyard.State == 31 && neighbourhood.State == localState, "All mission progress restores across separate player processes");
                foreach (var cat in FindObjectsByType<CatCompanion>(FindObjectsSortMode.None))
                    Check(cat.Friendship == (cat.energetic ? 2 : 3) && cat.CompanionQuestComplete, cat.energetic ? "Lakitu friendship restores after restart" : "Rio friendship restores after restart");
                Check(Mathf.Abs(GameSave.Data.volume - .42f) < .001f && Mathf.Abs(GameSave.Data.musicVolume - .27f) < .001f && Mathf.Abs(GameSave.Data.cameraScale - 1.1f) < .001f&&GameSave.Data.cameraMode==(int)CameraPreset.Explore, "Music, effects, camera distance and camera preset restore after restart");
                Check(controls.keyboard[(int)DogAction.Interact] == KeyCode.F && GameSave.Data.playStationLabels && controls.gamepad[(int)DogAction.Menu] == KeyCode.JoystickButton9, "Custom key bindings and PlayStation preset restore after restart");
                Check(GetComponent<FrontEndMenu>().ShowingTitle && Time.timeScale == 0, "Saved adventure opens at the paused title");
                GetComponent<FrontEndMenu>().StartAdventure(false); yield return new WaitForSecondsRealtime(.15f);
                Check(quest.CurrentStage == 4 && neighbourhood.State == localState && Time.timeScale > 0, "Continue resumes saved progress without resetting it");
                GameSave.PersistenceEnabled = false; GameSave.ClearAcceptanceSave();
            }
            Check(errors.Count == 0, "No runtime errors during isolated save verification");
        }
        private IEnumerator MappedTour()
        {
            var map = MappedNeighbourhood.Current; var director = GetComponent<NeighbourhoodDirector>();
            yield return MappedMenu();
            yield return Drive(0, 2.6f);
            foreach (var point in map.tour)
            {
                yield return Drive(point.x, point.z, true);
                if (point.stop == 0) continue;
                yield return Interact(Stop((NeighbourhoodStop)point.stop));
                Check(director.Has((NeighbourhoodStop)point.stop), "Mapped walking stop " + point.stop + " visited using production gamepad controls");
                if (point.stop == 4) yield return Shot("local-park");
                if (point.stop == 16) yield return Shot("local-school");
            }
            Check(director.Complete, "Completed the mapped park and school patrol and returned home");
            GameSave.Capture(); Check(GameSave.Data.neighbourhoodState == 31, "Four-stop local patrol saves independently of existing quests");
            yield return Drive(0, -1); Check(dog.transform.position.z > -2, "Walked back inside after the real-street tour");
        }
        private IEnumerator MappedMenu()
        {
            var map = MappedNeighbourhood.Current; var director = GetComponent<NeighbourhoodDirector>();
            Check(map.IncludesSchool && GameObject.Find("School | stylized exterior"), "Mapped school exterior and walking stop are present");
            InputSystem.QueueStateEvent(keyboard, new KeyboardState(Key.M)); yield return new WaitForSecondsRealtime(.15f);
            InputSystem.QueueStateEvent(keyboard, new KeyboardState()); yield return new WaitForSecondsRealtime(.1f);
            Check(director.MapExpanded, "Keyboard opens the full local street map"); yield return Shot("local-map");
            InputSystem.QueueStateEvent(keyboard, new KeyboardState(Key.M)); yield return new WaitForSecondsRealtime(.1f);
            InputSystem.QueueStateEvent(keyboard, new KeyboardState()); yield return new WaitForSecondsRealtime(.1f);
            Check(!director.MapExpanded, "Keyboard closes the full local street map");
        }
        private IEnumerator CatPortraits()
        {
            var cats = FindObjectsByType<CatVisual>(FindObjectsSortMode.None);
            Check(cats.Length == 2, "Both cats have distinct photo-guided visuals");
            var camera = Camera.main; var follow = camera.GetComponent<CheweenieCamera>();
            Vector3 position = camera.transform.position; Quaternion rotation = camera.transform.rotation;
            bool followEnabled = follow.enabled, questEnabled = quest.enabled;
            var neighbourhood = GetComponent<NeighbourhoodDirector>(); bool neighbourhoodEnabled = neighbourhood.enabled;
            var neighbourhoodWorld = FindFirstObjectByType<NeighbourhoodBuilder>(); bool worldActive = neighbourhoodWorld.gameObject.activeSelf;
            float timeScale = Time.timeScale; Time.timeScale = 0; follow.enabled = false; quest.enabled = false; neighbourhood.enabled = false;
            neighbourhoodWorld.gameObject.SetActive(false); // Keep private street geometry out of shareable cat portraits.
            foreach (var cat in cats)
            {
                var body = cat.GetComponent<Rigidbody>(); Quaternion original = body.rotation;
                var interpolation = body.interpolation; body.interpolation = RigidbodyInterpolation.None;
                body.rotation = Quaternion.identity; cat.transform.rotation = Quaternion.identity; Physics.SyncTransforms();
                var head = cat.transform.Find(cat.look + " | reference-based cat rig/Reference-patterned head");
                Check(head && head.GetComponent<Renderer>().sharedMaterial.mainTexture, cat.look + " patterned face is bundled and rendered");
                portraitLabel = cat.look == CatLook.Lakitu ? "LAKITU / KEETZ" : "RIO / MISS PRISS";
                camera.transform.position = cat.transform.position + new Vector3(0, .58f, 1.85f); camera.transform.LookAt(cat.transform.position + Vector3.up * .1f);
                yield return new WaitForSecondsRealtime(.12f); yield return Shot(cat.look == CatLook.Lakitu ? "lakitu-portrait" : "rio-portrait");
                body.rotation = original; cat.transform.rotation = original; body.interpolation = interpolation; Physics.SyncTransforms();
            }
            portraitLabel = null; camera.transform.SetPositionAndRotation(position, rotation);
            neighbourhoodWorld.gameObject.SetActive(worldActive);
            follow.enabled = followEnabled; quest.enabled = questEnabled; neighbourhood.enabled = neighbourhoodEnabled; Time.timeScale = timeScale;
        }
        private IEnumerator MartyPortrait()
        {
            var camera=Camera.main;var follow=camera.GetComponent<CheweenieCamera>();
            Vector3 position=camera.transform.position;Quaternion rotation=camera.transform.rotation;
            var hidden=new List<Renderer>();foreach(var renderer in FindObjectsByType<Renderer>(FindObjectsSortMode.None))
                if(renderer.enabled&&!renderer.transform.IsChildOf(dog.transform)){hidden.Add(renderer);renderer.enabled=false;}
            bool questEnabled=quest.enabled;var neighbourhood=GetComponent<NeighbourhoodDirector>();bool mapEnabled=neighbourhood.enabled;
            quest.enabled=false;neighbourhood.enabled=false;follow.enabled=false;
            var body=dog.GetComponent<Rigidbody>();var interpolation=body.interpolation;body.interpolation=RigidbodyInterpolation.None;
            Quaternion original=body.rotation;body.rotation=Quaternion.identity;dog.transform.rotation=Quaternion.identity;Physics.SyncTransforms();
            float timeScale=Time.timeScale;Time.timeScale=0;portraitLabel="MARTY / MR. MARTY";
            var clear=camera.clearFlags;Color background=camera.backgroundColor;camera.clearFlags=CameraClearFlags.SolidColor;camera.backgroundColor=new Color(.13f,.19f,.23f);
            camera.transform.position=dog.transform.position+new Vector3(1.65f,.55f,2.5f);camera.transform.LookAt(dog.transform.position+new Vector3(0,-.12f,.1f));
            yield return Shot("marty-portrait");
            Check(dog.GetComponentsInChildren<Rigidbody>().Length==1&&dog.GetComponentsInChildren<Collider>().Length==1,"Marty character keeps the original gameplay Rigidbody and capsule");
            portraitLabel=null;camera.clearFlags=clear;camera.backgroundColor=background;camera.transform.SetPositionAndRotation(position,rotation);
            body.rotation=original;dog.transform.rotation=original;body.interpolation=interpolation;Physics.SyncTransforms();
            foreach(var renderer in hidden)if(renderer)renderer.enabled=true;
            quest.enabled=questEnabled;neighbourhood.enabled=mapEnabled;follow.enabled=true;Time.timeScale=timeScale;
        }
        private IEnumerator SceneryPortraits()
        {
            var camera=Camera.main;var follow=camera.GetComponent<CheweenieCamera>();Vector3 position=camera.transform.position;Quaternion rotation=camera.transform.rotation;
            var neighbourhood=GetComponent<NeighbourhoodDirector>();var mapWorld=FindFirstObjectByType<NeighbourhoodBuilder>();bool active=mapWorld.gameObject.activeSelf;
            bool q=quest.enabled,n=neighbourhood.enabled;float scale=Time.timeScale;Time.timeScale=0;quest.enabled=false;neighbourhood.enabled=false;follow.enabled=false;mapWorld.gameObject.SetActive(false);
            var farProps=new List<GameObject>();var art=transform.Find("Retained cozy scenery | visual layer");if(art)foreach(Transform prop in art)if(prop.position.magnitude>40&&prop.gameObject.activeSelf){farProps.Add(prop.gameObject);prop.gameObject.SetActive(false);}
            var backdrop=GameObject.CreatePrimitive(PrimitiveType.Cube);backdrop.name="Temporary neutral showcase ground";backdrop.transform.position=new Vector3(0,-.45f,0);backdrop.transform.localScale=new Vector3(400,.5f,400);backdrop.GetComponent<Collider>().enabled=false;backdrop.GetComponent<Renderer>().sharedMaterial=Resources.Load<SceneryPalette>("SceneryPalette").grass;var tiles=new MaterialPropertyBlock();tiles.SetVector("_MainTex_ST",new Vector4(100,100,0,0));backdrop.GetComponent<Renderer>().SetPropertyBlock(tiles);
            portraitLabel="COZY HEADQUARTERS";camera.transform.position=new Vector3(12,13,-16);camera.transform.LookAt(new Vector3(0,.2f,3));yield return Shot("headquarters-art");
            portraitLabel="LIVING ROOM / CAT CREW";camera.transform.position=new Vector3(5.7f,3.3f,-1.2f);camera.transform.LookAt(new Vector3(-1.5f,.8f,5.5f));yield return Shot("living-room-art");
            portraitLabel="BACKYARD ADVENTURE";camera.transform.position=new Vector3(22,13,-29);camera.transform.LookAt(new Vector3(0,.5f,-12));yield return Shot("backyard-art");
            Destroy(backdrop);foreach(var prop in farProps)if(prop)prop.SetActive(true);portraitLabel=null;camera.transform.SetPositionAndRotation(position,rotation);mapWorld.gameObject.SetActive(active);quest.enabled=q;neighbourhood.enabled=n;follow.enabled=true;Time.timeScale=scale;
        }
        private IEnumerator CameraControllerProbe()
        {
            var camera=Camera.main.GetComponent<CheweenieCamera>();GameSave.Data.cameraMode=0;camera.SnapToTarget();
            Pad(Vector2.zero,GamepadButton.RightStick);yield return new WaitForSecondsRealtime(.1f);Pad(Vector2.zero);yield return new WaitForSecondsRealtime(.1f);
            Check(camera.Preset==CameraPreset.Explore,"Controller R3 cycles from Cozy to Explore camera");
            float yaw=camera.Yaw;InputSystem.QueueStateEvent(pad,new GamepadState{rightStick=new Vector2(.8f,.25f)});yield return new WaitForSecondsRealtime(.25f);Pad(Vector2.zero);yield return null;
            Check(Mathf.Abs(Mathf.DeltaAngle(yaw,camera.Yaw))>10,"Right stick orbits the camera without a mouse");
            Vector3 start=dog.transform.position,forward=camera.ToWorldMovement(Vector3.forward);Pad(Vector2.up);yield return new WaitForSecondsRealtime(.25f);Pad(Vector2.zero);yield return new WaitForSecondsRealtime(.2f);
            Check(Vector3.Dot(dog.transform.position-start,forward)>.2f,"Explore camera uses controller movement relative to the view");
            Pad(Vector2.zero,GamepadButton.RightStick);yield return new WaitForSecondsRealtime(.1f);Pad(Vector2.zero);yield return new WaitForSecondsRealtime(.1f);Check(camera.Preset==CameraPreset.Low,"Controller R3 selects the low camera preset");
            var map=GetComponent<NeighbourhoodDirector>();bool mapHud=map.enabled;map.enabled=false;yield return Shot("camera-low");map.enabled=mapHud;
            Pad(Vector2.zero,GamepadButton.RightShoulder);yield return new WaitForSecondsRealtime(.1f);Pad(Vector2.zero);yield return new WaitForSecondsRealtime(.1f);Check(Mathf.Abs(Mathf.DeltaAngle(camera.Yaw,dog.transform.eulerAngles.y))<5,"Controller RB/R1 recenters behind Marty");
            Pad(Vector2.zero,GamepadButton.RightStick);yield return new WaitForSecondsRealtime(.1f);Pad(Vector2.zero);yield return new WaitForSecondsRealtime(.1f);Check(camera.Preset==CameraPreset.Cozy,"Controller camera cycle returns to Cozy overview");
            controls.ApplyControllerPreset(true);Pad(Vector2.zero,GamepadButton.Start);yield return new WaitForSecondsRealtime(.1f);Pad(Vector2.zero);yield return new WaitForSecondsRealtime(.1f);Check(GetComponent<CozySettings>().IsOpen,"Controller Options opens the pause menu");
            Pad(Vector2.zero,GamepadButton.East);yield return new WaitForSecondsRealtime(.1f);Pad(Vector2.zero);yield return new WaitForSecondsRealtime(.15f);Check(!GetComponent<CozySettings>().IsOpen&&Time.timeScale>0,"Controller Circle backs out and resumes without a mouse");controls.ApplyControllerPreset(false);
            var gait=dog.GetComponentInChildren<GroundedGait>();Check(gait&&gait.Travelled>.3f&&gait.Steps>4,"Marty's distance-driven gait produces ground contacts while walking");
        }
        private IEnumerator MotionDemo()
        {
            yield return Drive(0,-9);var camera=Camera.main.GetComponent<CheweenieCamera>();GameSave.Data.cameraMode=(int)CameraPreset.Explore;camera.SnapToTarget();camera.SetOrbit(0,18);
            // Front view of a slow walk, away from the private street layer and map HUD.
            var map=GetComponent<NeighbourhoodDirector>();bool enabled=map.enabled;map.enabled=false;portraitLabel="MARTY / GROUNDED WALK";
            var mapWorld=FindFirstObjectByType<NeighbourhoodBuilder>();bool mapActive=mapWorld.gameObject.activeSelf;mapWorld.gameObject.SetActive(false);var hidden=new List<GameObject>();var art=transform.Find("Retained cozy scenery | visual layer");if(art)foreach(Transform prop in art)if(prop.position.magnitude>40&&prop.gameObject.activeSelf){hidden.Add(prop.gameObject);prop.gameObject.SetActive(false);}
            Pad(new Vector2(0,-.4f));yield return new WaitForSecondsRealtime(.4f);
            for(int i=0;i<24;i++){yield return new WaitForEndOfFrame();ScreenCapture.CaptureScreenshot(Path.Combine(Path.GetDirectoryName(report),"motion-"+i.ToString("00")+".png"));yield return new WaitForSecondsRealtime(.08f);}
            Pad(Vector2.zero);portraitLabel=null;map.enabled=enabled;mapWorld.gameObject.SetActive(mapActive);foreach(var prop in hidden)if(prop)prop.SetActive(true);GameSave.Data.cameraMode=0;camera.SnapToTarget();
        }
    }
}
