using System.IO;
using UnityEditor;
using UnityEngine;

namespace CodenameCheweenie.Editor
{
    public static class LocomotionAssetBuilder
    {
        [MenuItem("Cheweenie/Art/Create missing locomotion profiles")]
        public static void EnsureProfiles()
        {
            Directory.CreateDirectory("Assets/Resources/Motion");AssetDatabase.Refresh();
            foreach(string name in new[]{"Marty","Lakitu","Rio"})
            {
                string path="Assets/Resources/Motion/"+name+".asset";if(AssetDatabase.LoadAssetAtPath<LocomotionTuning>(path))continue;
                var tuning=ScriptableObject.CreateInstance<LocomotionTuning>();
                if(name!="Marty"){tuning.walkStride=.36f;tuning.trotStride=.62f;tuning.runStride=.85f;tuning.trotSpeed=1.3f;tuning.runSpeed=3;tuning.pawLift=.065f;tuning.maxReach=.25f;tuning.bodyBob=.008f;tuning.bodyRoll=.8f;tuning.maxStretch=1.35f;}
                AssetDatabase.CreateAsset(tuning,path);
            }
            AssetDatabase.SaveAssets();
        }
    }
}
