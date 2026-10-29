using System;
using System.IO;
using UnityEditor;
using UnityEditor.Build;
using UnityEditor.Build.Reporting;
using UnityEditor.SceneManagement;
using UnityEngine;
namespace CodenameCheweenie.Editor
{
    public static class VisualLabBuild
    {
        [MenuItem("Cheweenie/Visual Lab/Build separate Windows experiment")]
        public static void Build()
        {
            MartyModelBuilder.EnsureModel();CatModelBuilder.EnsureModels();SceneryAssetBuilder.EnsureAssets();LocomotionAssetBuilder.EnsureProfiles();CheweenieBuild.PrepareMaterials();
            Directory.CreateDirectory("Assets/Resources/VisualLab");AssetDatabase.Refresh();
            var alternative=AssetDatabase.LoadAssetAtPath<GameObject>("Assets/Art/Marty/MartyPhotoGuided.prefab");
            if(!alternative)throw new InvalidOperationException("Retained photo-guided Marty alternative is missing.");
            if(!AssetDatabase.LoadAssetAtPath<GameObject>("Assets/Resources/VisualLab/MartyAnatomy.prefab"))PrefabUtility.SaveAsPrefabAsset(alternative,"Assets/Resources/VisualLab/MartyAnatomy.prefab");
            AssetDatabase.SaveAssets();
            PlayerSettings.companyName="CheweenieGraphicsLab";PlayerSettings.productName=BrandTheme.Title+" Visual Lab";PlayerSettings.bundleVersion="0.1.0-visual-lab";
            PlayerSettings.colorSpace=ColorSpace.Linear;PlayerSettings.forceSingleInstance=true;PlayerSettings.defaultScreenWidth=1280;PlayerSettings.defaultScreenHeight=720;PlayerSettings.fullScreenMode=FullScreenMode.Windowed;
            PlayerSettings.SetUseDefaultGraphicsAPIs(BuildTarget.StandaloneWindows64,false);PlayerSettings.SetGraphicsAPIs(BuildTarget.StandaloneWindows64,new[]{UnityEngine.Rendering.GraphicsDeviceType.Direct3D11});
            PlayerSettings.SetScriptingBackend(NamedBuildTarget.Standalone,ScriptingImplementation.Mono2x);
            Directory.CreateDirectory("Assets/Generated");Directory.CreateDirectory("builds/VisualLab");
            var scene=EditorSceneManager.NewScene(NewSceneSetup.EmptyScene,NewSceneMode.Single);EditorSceneManager.SaveScene(scene,"Assets/Generated/VisualLab.unity");
            var result=BuildPipeline.BuildPlayer(new BuildPlayerOptions{scenes=new[]{"Assets/Generated/VisualLab.unity"},locationPathName="builds/VisualLab/CheweenieLab.exe",target=BuildTarget.StandaloneWindows64,options=BuildOptions.CompressWithLz4HC});
            if(result.summary.result!=BuildResult.Succeeded)throw new InvalidOperationException("Visual lab build failed: "+result.summary.result);
            Debug.Log("Separate graphics lab built with isolated company/product/save storage.");
        }
    }
}
