using UnityEngine;

namespace CodenameCheweenie
{
    [CreateAssetMenu(menuName="Cheweenie/Locomotion Tuning")]
    public sealed class LocomotionTuning:ScriptableObject
    {
        public float walkStride=.72f,trotStride=1.2f,runStride=1.8f;
        public float trotSpeed=2.2f,runSpeed=5.5f,pawLift=.11f,maxReach=.42f;
        public float bodyBob=.015f,bodyRoll=1.2f,maxStretch=1.5f;
        public static float LegPhase(float cycle,int leg,float speed,LocomotionTuning tuning)
        {
            float offset=speed>=tuning.runSpeed?(leg==0?0:leg==1?.12f:leg==2?.58f:.70f)
                :speed>=tuning.trotSpeed?(leg==0||leg==3?0:.5f)
                :leg==0?0:leg==1?.5f:leg==2?.75f:.25f;
            return Mathf.Repeat(cycle+offset,1);
        }
        public static float StanceFraction(float speed,LocomotionTuning tuning)=>speed>=tuning.runSpeed?.35f:speed>=tuning.trotSpeed?.52f:.68f;
    }
}
