using NUnit.Framework;
using UnityEngine;

namespace CodenameCheweenie.Tests
{
    public sealed class CameraGaitRulesTests
    {
        [Test] public void WalkTrotAndRunUseDifferentFootfallTiming()
        {
            var tuning=ScriptableObject.CreateInstance<LocomotionTuning>();
            try
            {
                Assert.AreEqual(.75f,LocomotionTuning.LegPhase(0,2,1,tuning));
                Assert.AreEqual(LocomotionTuning.LegPhase(.1f,0,3,tuning),LocomotionTuning.LegPhase(.1f,3,3,tuning));
                Assert.AreNotEqual(LocomotionTuning.LegPhase(.1f,0,7,tuning),LocomotionTuning.LegPhase(.1f,1,7,tuning));
                Assert.Greater(LocomotionTuning.StanceFraction(1,tuning),LocomotionTuning.StanceFraction(7,tuning));
                Assert.IsNotNull(Resources.Load<LocomotionTuning>("Motion/Marty"));Assert.IsNotNull(Resources.Load<LocomotionTuning>("Motion/Lakitu"));Assert.IsNotNull(Resources.Load<LocomotionTuning>("Motion/Rio"));
            }
            finally{Object.DestroyImmediate(tuning);}
        }
        [Test] public void OldSavesDefaultToCozyAndCameraMovementPreservesStickMagnitude()
        {
            var data=JsonUtility.FromJson<SaveData>("{\"version\":1,\"stage\":2}");Assert.AreEqual(0,data.cameraMode);
            var go=new GameObject("Camera rules");var camera=go.AddComponent<CheweenieCamera>();
            GameSave.Load();int original=GameSave.Data.cameraMode;
            try{GameSave.Data.cameraMode=0;Vector3 input=new Vector3(.2f,0,.4f);Assert.AreEqual(input,camera.ToWorldMovement(input));GameSave.Data.cameraMode=1;Assert.AreEqual(input.magnitude,camera.ToWorldMovement(input).magnitude,.0001f);}
            finally{GameSave.Data.cameraMode=original;Object.DestroyImmediate(go);}
        }
    }
}
