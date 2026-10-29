using System.Collections;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.LowLevel;
using UnityEngine.TestTools;

namespace CodenameCheweenie.Tests
{
    public sealed class ControllerCameraMotionTests
    {
        private Gamepad pad;
        private InputSettings.BackgroundBehavior background;
#if UNITY_EDITOR
        private InputSettings.EditorInputBehaviorInPlayMode editorBehavior;
#endif
        private IEnumerator Setup()
        {
            GameSave.PersistenceEnabled=false;GameSave.Load();GameSave.Data.cameraMode=0;
            background=InputSystem.settings.backgroundBehavior;InputSystem.settings.backgroundBehavior=InputSettings.BackgroundBehavior.IgnoreFocus;
#if UNITY_EDITOR
            editorBehavior=InputSystem.settings.editorInputBehaviorInPlayMode;InputSystem.settings.editorInputBehaviorInPlayMode=InputSettings.EditorInputBehaviorInPlayMode.AllDeviceInputAlwaysGoesToGameView;
#endif
            pad=InputSystem.AddDevice<Gamepad>();CheweenieBootstrap.Build();yield return null;
        }
        private IEnumerator Press(GamepadButton button)
        {InputSystem.QueueStateEvent(pad,new GamepadState().WithButton(button));yield return new WaitForSecondsRealtime(.08f);InputSystem.QueueStateEvent(pad,new GamepadState());yield return new WaitForSecondsRealtime(.08f);}
        [UnityTest] public IEnumerator ControllerCanStartNavigateCamerasOrbitAndCancelWithoutMouse()
        {
            yield return Setup();var front=Object.FindFirstObjectByType<FrontEndMenu>();var settings=Object.FindFirstObjectByType<CozySettings>();
            yield return Press(GamepadButton.East);Assert.IsTrue(front.ShowingTitle,"B/Circle cannot accidentally start the adventure.");
            yield return Press(GamepadButton.South);Assert.IsFalse(front.ShowingTitle);yield return Press(GamepadButton.Start);Assert.IsTrue(settings.IsOpen);
            yield return Press(GamepadButton.DpadDown);yield return Press(GamepadButton.DpadDown);yield return Press(GamepadButton.South);
            var camera=Camera.main.GetComponent<CheweenieCamera>();Assert.AreEqual(CameraPreset.Explore,camera.Preset);Assert.AreEqual(0,Time.timeScale);
            float pausedYaw=camera.Yaw;InputSystem.QueueStateEvent(pad,new GamepadState{rightStick=Vector2.right});yield return new WaitForSecondsRealtime(.15f);InputSystem.QueueStateEvent(pad,new GamepadState());yield return null;Assert.AreEqual(pausedYaw,camera.Yaw,"Pause menus must not orbit the world.");
            yield return Press(GamepadButton.East);Assert.IsFalse(settings.IsOpen);Assert.AreEqual(1,Time.timeScale);
            float yaw=camera.Yaw;InputSystem.QueueStateEvent(pad,new GamepadState{rightStick=Vector2.right});yield return new WaitForSecondsRealtime(.3f);InputSystem.QueueStateEvent(pad,new GamepadState());yield return null;
            Assert.Greater(Mathf.Abs(Mathf.DeltaAngle(yaw,camera.Yaw)),15);
            var dog=Object.FindFirstObjectByType<MartyController>();Vector3 start=dog.transform.position,direction=camera.ToWorldMovement(Vector3.forward);
            InputSystem.QueueStateEvent(pad,new GamepadState{leftStick=Vector2.up});yield return new WaitForSecondsRealtime(.35f);InputSystem.QueueStateEvent(pad,new GamepadState());yield return new WaitForSecondsRealtime(.2f);
            Assert.Greater(Vector3.Dot(dog.transform.position-start,direction),.3f);
            yield return Press(GamepadButton.RightStick);Assert.AreEqual(CameraPreset.Low,camera.Preset);yield return Press(GamepadButton.RightShoulder);Assert.Less(Mathf.Abs(Mathf.DeltaAngle(camera.Yaw,dog.transform.eulerAngles.y)),5);
            yield return Press(GamepadButton.RightStick);Assert.AreEqual(CameraPreset.Cozy,camera.Preset);
            var input=dog.GetComponent<MartyInput>();input.ApplyControllerPreset(true);yield return Press(GamepadButton.Start);Assert.IsTrue(settings.IsOpen);
            for(int i=0;i<7;i++)yield return Press(GamepadButton.DpadUp);yield return Press(GamepadButton.South);Assert.AreEqual(KeyCode.JoystickButton1,input.gamepad[(int)DogAction.Jump],"Reset bindings retains Cross on the PlayStation preset.");
            yield return Press(GamepadButton.East);Assert.IsFalse(settings.IsOpen);
            LogAssert.NoUnexpectedReceived();
        }
        [UnityTest] public IEnumerator AnalogWalkIsSlowerThanFullStickAndPawsStayPlantedDuringStance()
        {
            yield return Setup();Object.FindFirstObjectByType<FrontEndMenu>().StartAdventure(true);yield return new WaitForSecondsRealtime(.2f);
            var dog=Object.FindFirstObjectByType<MartyController>();var gait=dog.GetComponentInChildren<GroundedGait>();Assert.IsNotNull(gait);
            Vector3 start=dog.transform.position;InputSystem.QueueStateEvent(pad,new GamepadState{leftStick=new Vector2(0,.4f)});
            int[] ids=new int[4];Vector3[] feet=new Vector3[4];bool[] planted=new bool[4];int pairs=0;float slip=0;
            float until=Time.realtimeSinceStartup+1;
            while(Time.realtimeSinceStartup<until)
            {
                yield return null;
                for(int i=0;i<4;i++){Vector3 current=gait.SolePosition(i);if(planted[i]&&gait.IsPlanted(i)&&ids[i]==gait.ContactId(i)){pairs++;slip=Mathf.Max(slip,Vector3.Distance(current,feet[i]));}feet[i]=current;ids[i]=gait.ContactId(i);planted[i]=gait.IsPlanted(i);}
            }
            float slow=dog.transform.position.z-start.z;InputSystem.QueueStateEvent(pad,new GamepadState());yield return new WaitForSecondsRealtime(.25f);start=dog.transform.position;
            InputSystem.QueueStateEvent(pad,new GamepadState{leftStick=Vector2.up});yield return new WaitForSecondsRealtime(1);InputSystem.QueueStateEvent(pad,new GamepadState());
            float fast=dog.transform.position.z-start.z;Assert.Greater(slow,.3f);Assert.Greater(fast,slow*1.6f);Assert.Greater(pairs,10);Assert.Less(slip,.025f);Assert.Greater(gait.Steps,4);
            yield return new WaitForSecondsRealtime(.4f);float phase=gait.Cycle;yield return new WaitForSecondsRealtime(.2f);Assert.AreEqual(phase,gait.Cycle,.01f,"An idle pet must not skate through its gait cycle.");
            LogAssert.NoUnexpectedReceived();
        }
        [UnityTearDown] public IEnumerator Cleanup()
        {
            if(pad!=null)InputSystem.RemoveDevice(pad);InputSystem.settings.backgroundBehavior=background;
#if UNITY_EDITOR
            InputSystem.settings.editorInputBehaviorInPlayMode=editorBehavior;
#endif
            var quest=Object.FindFirstObjectByType<QuestDirector>();if(quest)Object.Destroy(quest.gameObject);yield return null;GameSave.Reload();GameSave.PersistenceEnabled=true;Time.timeScale=1;
        }
    }
}
