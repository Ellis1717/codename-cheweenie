using System.Collections;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.LowLevel;
using UnityEngine.TestTools;

namespace CodenameCheweenie.Tests
{
    public sealed class InputDeviceTests
    {
        private Keyboard keyboard;
        private Gamepad pad;
        private Mouse mouse;
        private InputSettings.BackgroundBehavior background;
#if UNITY_EDITOR
        private InputSettings.EditorInputBehaviorInPlayMode editorBehavior;
#endif
        [UnityTest] public IEnumerator VirtualKeyboardAndControllerDriveTheProductionControls()
        {
            GameSave.PersistenceEnabled = false;
            background = InputSystem.settings.backgroundBehavior;
            InputSystem.settings.backgroundBehavior = InputSettings.BackgroundBehavior.IgnoreFocus;
#if UNITY_EDITOR
            editorBehavior = InputSystem.settings.editorInputBehaviorInPlayMode;
            InputSystem.settings.editorInputBehaviorInPlayMode = InputSettings.EditorInputBehaviorInPlayMode.AllDeviceInputAlwaysGoesToGameView;
#endif
            keyboard = InputSystem.AddDevice<Keyboard>(); pad = InputSystem.AddDevice<Gamepad>();
            CheweenieBootstrap.Build(); yield return null;
            var dog = Object.FindFirstObjectByType<MartyController>(); var input = dog.GetComponent<MartyInput>();
            Object.FindFirstObjectByType<FrontEndMenu>().StartAdventure(true);
            yield return new WaitForSecondsRealtime(.2f);
            Vector3 initial = dog.transform.position;
            InputSystem.QueueStateEvent(keyboard, new KeyboardState(Key.W));
            yield return new WaitForSecondsRealtime(.35f);
            InputSystem.QueueStateEvent(keyboard, new KeyboardState());
            Assert.Greater(dog.transform.position.z, initial.z + .5f);
            yield return new WaitForSecondsRealtime(.15f);
            float y = dog.transform.position.y;
            InputSystem.QueueStateEvent(keyboard, new KeyboardState(Key.Space));
            yield return new WaitForSecondsRealtime(.18f);
            Assert.Greater(dog.transform.position.y, y + .3f);
            InputSystem.QueueStateEvent(keyboard, new KeyboardState());
            yield return new WaitForSecondsRealtime(1);
            input.ApplyControllerPreset(false); initial = dog.transform.position;
            InputSystem.QueueStateEvent(pad, new GamepadState { leftStick = Vector2.right });
            yield return new WaitForSecondsRealtime(.35f);
            Assert.Greater(dog.transform.position.x, initial.x + .5f);
            InputSystem.QueueStateEvent(pad, new GamepadState().WithButton(GamepadButton.West)); yield return null;
            Assert.IsTrue(input.Held(DogAction.Interact), "Xbox X uses the named west button");
            InputSystem.QueueStateEvent(pad, new GamepadState()); yield return null;
            input.ApplyControllerPreset(true);
            InputSystem.QueueStateEvent(pad, new GamepadState().WithButton(GamepadButton.South)); yield return null;
            Assert.IsTrue(input.Held(DogAction.Jump), "PlayStation Cross uses the named south button");
            Assert.IsFalse(GameControls.GetKey(KeyCode.JoystickButton0), "Square is not Cross");
            InputSystem.QueueStateEvent(pad, new GamepadState()); yield return null;
            InputSystem.QueueStateEvent(pad, new GamepadState().WithButton(GamepadButton.Start));
            yield return new WaitForSecondsRealtime(.1f);
            var settings = Object.FindFirstObjectByType<CozySettings>();
            Assert.IsTrue(settings.IsOpen, "PlayStation Options opens settings");
            InputSystem.QueueStateEvent(pad, new GamepadState()); settings.Close();
            yield return new WaitForSecondsRealtime(.15f);
            var quest = Object.FindFirstObjectByType<QuestDirector>(); quest.Restore(3);
            var body = dog.GetComponent<Rigidbody>();
            body.position = new Vector3(0, .95f, 5.2f); dog.transform.position = body.position;
            body.linearVelocity = Vector3.zero; Physics.SyncTransforms();
            input.ApplyControllerPreset(false); yield return new WaitForSecondsRealtime(.2f);
            InputSystem.QueueStateEvent(pad, new GamepadState().WithButton(GamepadButton.North));
            yield return new WaitForSecondsRealtime(.15f);
            Assert.AreEqual(4, quest.CurrentStage, "Cozy validates before its Rigidbody movement can obstruct the reach check");
            Assert.IsTrue(dog.IsCozy);
            InputSystem.QueueStateEvent(pad, new GamepadState());
            InputSystem.QueueStateEvent(keyboard, new KeyboardState(Key.PrintScreen)); yield return null;
            Assert.IsTrue(GameControls.GetKey(KeyCode.Print), "Existing Print bindings map to PrintScreen");
            InputSystem.QueueStateEvent(keyboard, new KeyboardState());
            mouse = InputSystem.AddDevice<Mouse>();
            InputSystem.QueueStateEvent(mouse, new MouseState { position = new Vector2(120, Screen.height - 80) }.WithButton(MouseButton.Left));
            yield return null;
            var matrix = Matrix4x4.Scale(Vector3.one * 2);
            Assert.Less(Vector2.Distance(new Vector2(60, 40), GameControls.PointerPosition(matrix)), .01f);
            Assert.IsTrue(GameControls.TryPointerClick(new Rect(50, 30, 20, 20), matrix));
            Assert.IsFalse(GameControls.TryPointerClick(new Rect(50, 30, 20, 20), matrix), "One click cannot activate two GUI passes");
            Assert.AreEqual(0, GameControls.GetAxisRaw("UnknownAxis"));
            var devices = new System.Collections.Generic.List<InputDevice>();
            foreach (var device in InputSystem.devices) if (device is Keyboard || device is Gamepad) devices.Add(device);
            try
            {
                foreach (var device in devices) InputSystem.RemoveDevice(device);
                Assert.IsFalse(GameControls.GetKey(KeyCode.Space));
                Assert.IsFalse(GameControls.GetKeyDown(KeyCode.JoystickButton0));
                Assert.AreEqual(0, GameControls.GetAxisRaw("DogStickX"));
                Assert.AreEqual(0, GameControls.GetAxisRaw("DogStickY"));
            }
            finally { foreach (var device in devices) InputSystem.AddDevice(device); }
            LogAssert.NoUnexpectedReceived();
        }
        [UnityTearDown] public IEnumerator Cleanup()
        {
            if (keyboard != null) InputSystem.RemoveDevice(keyboard);
            if (pad != null) InputSystem.RemoveDevice(pad);
            if (mouse != null) InputSystem.RemoveDevice(mouse);
            InputSystem.settings.backgroundBehavior = background;
#if UNITY_EDITOR
            InputSystem.settings.editorInputBehaviorInPlayMode = editorBehavior;
#endif
            var quest = Object.FindFirstObjectByType<QuestDirector>(); if (quest) Object.Destroy(quest.gameObject);
            yield return null; GameSave.Reload(); GameSave.PersistenceEnabled = true; Time.timeScale = 1;
        }
    }
}
