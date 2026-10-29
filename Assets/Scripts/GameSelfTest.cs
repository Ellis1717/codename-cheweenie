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
            public float seconds, averageFramesPerSecond;
            public int homeStage, backyardState, neighbourhoodState, materialCount;
            public string[] checks, errors;
        }
        private readonly List<string> checks = new List<string>();
        private readonly List<string> errors = new List<string>();
        private Gamepad pad;
        private Keyboard keyboard;
        private MartyController dog;
        private MartyInput controls;
        private QuestDirector quest;
        private BackyardDirector backyard;
        private string report;
        private float started;
        private int firstFrame;
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
                if (failure != null || Time.realtimeSinceStartup - started > 240)
                { errors.Add(failure != null ? failure.ToString() : "Self-test exceeded 240 seconds"); Finish(false); yield break; }
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
                averageFramesPerSecond = (Time.frameCount - firstFrame) / Mathf.Max(.01f, Time.realtimeSinceStartup - started),
                homeStage = quest ? quest.CurrentStage : -1, backyardState = backyard ? backyard.State : -1,
                neighbourhoodState = GetComponent<NeighbourhoodDirector>() ? GetComponent<NeighbourhoodDirector>().State : -1,
                materialCount = Resources.FindObjectsOfTypeAll<Material>().Length, checks = checks.ToArray(), errors = errors.ToArray()
            };
            File.WriteAllText(report, JsonUtility.ToJson(result, true));
            Application.logMessageReceived -= Log;
            if (pad != null) InputSystem.RemoveDevice(pad);
            if (keyboard != null) InputSystem.RemoveDevice(keyboard);
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
        private IEnumerator Drive(float x, float z)
        {
            float until = Time.realtimeSinceStartup + 15;
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
                Pad(Vector2.ClampMagnitude(direction * 1.5f, 1)); yield return null;
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
            pad = InputSystem.AddDevice<Gamepad>(); keyboard = InputSystem.AddDevice<Keyboard>();
            dog = FindFirstObjectByType<MartyController>(); controls = dog.GetComponent<MartyInput>();
            quest = GetComponent<QuestDirector>(); backyard = GetComponent<BackyardDirector>();
            var frontEnd = GetComponent<FrontEndMenu>(); var settings = GetComponent<CozySettings>();
            yield return new WaitForSecondsRealtime(3.5f);
            while (!UnityEngine.Rendering.SplashScreen.isFinished) yield return null;
            Check(frontEnd.ShowingTitle && Time.timeScale == 0, "Branded title pauses gameplay");
            Check(Resources.Load<AudioClip>("Audio/Home") != null, "Music bundled in the player");
            yield return Shot("title");
            settings.OpenFromTitle(); yield return new WaitForSecondsRealtime(.15f);
            Check(settings.IsOpen && Time.timeScale == 0, "Settings opens from title"); yield return Shot("settings");
            settings.Close(); frontEnd.StartAdventure(true); controls.ApplyControllerPreset(false);
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
            yield return Drive(0, -7.8f); yield return Drive(-6.3f, -7.8f); yield return Drive(-6.3f, 4);
            Check(dog.transform.position.y > 4.2f, "Walked up the headquarters ramp to the second story"); yield return Shot("loft");
            yield return Drive(-6.3f, -7.8f); yield return Drive(0, -7.8f);
            yield return Drive(-4.8f, -1); yield return Drive(-4.8f, 2.5f);
            yield return Interact(Target(0)); Check(quest.CurrentStage == 1, "Greeted Lakitu using gamepad interaction");
            yield return Drive(-4.8f, -1); yield return Drive(0, -1); yield return Drive(0, 2.6f); yield return Drive(5, 2.6f);
            yield return Interact(Target(1)); Check(quest.CurrentStage == 2, "Greeted Rio");
            yield return Drive(0, 2.6f); yield return Drive(0, -8);
            yield return Interact(Target(2)); Check(quest.CurrentStage == 3, "Initial backyard sniff complete");
            yield return Drive(0, 2.6f); yield return Drive(0, 5.6f);
            yield return Interact(Target(3), true); Check(quest.CurrentStage == 4 && dog.IsCozy, "Cheeween Cozy completes the home mission");
            yield return Shot("cozy");
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
            yield return Drive(13.35f, -18); yield return Drive(13.35f, -8.3f); yield return Drive(0, -8.3f);
            yield return Drive(0, 2.6f); yield return Drive(5, 2.6f); yield return Interact(Target(1));
            Check(backyard.Complete, "Delivered ribbon to Rio; full adventure complete"); yield return Shot("complete");
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
            settings.OpenFromTitle(); settings.ReturnToTitle();
            Check(frontEnd.ShowingTitle && Time.timeScale == 0, "Return to title works in the built player");
            Check(errors.Count == 0, "No runtime errors during the full playthrough");
        }
    }
}
