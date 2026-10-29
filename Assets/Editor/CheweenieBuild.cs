using System;
using UnityEditor;
using UnityEditor.Build;
using UnityEditor.Build.Reporting;
using UnityEditor.SceneManagement;
using UnityEngine;

namespace CodenameCheweenie.Editor
{
    public static class CheweenieBuild
    {
        [MenuItem("Cheweenie/Validate project configuration")]
        public static void Validate()
        {
            var settings = AssetDatabase.LoadAllAssetsAtPath("ProjectSettings/ProjectSettings.asset");
            if (settings.Length > 0)
            {
                var handler = new SerializedObject(settings[0]).FindProperty("activeInputHandler");
                if (handler != null && handler.intValue != 1)
                    throw new InvalidOperationException("Set Active Input Handling to Input System Package (New), then restart Unity.");
            }
            if (!BuildPipeline.IsBuildTargetSupported(BuildTargetGroup.Standalone, BuildTarget.StandaloneWindows64))
                throw new InvalidOperationException("Install Windows Build Support through Unity Hub.");
            Debug.Log("Cheweenie configuration checks passed.");
        }
        [MenuItem("Cheweenie/Build Windows game")]
        public static void BuildWindows() { Build(false); }
        [MenuItem("Cheweenie/Build Windows validation player")]
        public static void BuildValidationPlayer() { Build(true); }
        [MenuItem("Cheweenie/Prepare runtime materials")]
        public static void PrepareMaterials()
        {
            System.IO.Directory.CreateDirectory("Assets/Resources/Branding");
            bool builtin = UnityEngine.Rendering.GraphicsSettings.currentRenderPipeline == null;
            MakeMaterial("Assets/Resources/Branding/World.mat", builtin ? "Standard" : "Universal Render Pipeline/Lit");
            MakeMaterial("Assets/Resources/Branding/Sparkles.mat", builtin ? "Particles/Standard Unlit" : "Universal Render Pipeline/Particles/Unlit");
            AssetDatabase.SaveAssets();
        }
        private static void MakeMaterial(string path, string shaderName)
        {
            var shader = Shader.Find(shaderName); if (!shader) throw new InvalidOperationException("Missing shader " + shaderName);
            var material = AssetDatabase.LoadAssetAtPath<Material>(path);
            if (!material) { material = new Material(shader); AssetDatabase.CreateAsset(material, path); }
            material.shader = shader; material.enableInstancing = true;
            if (material.HasProperty("_Glossiness")) material.SetFloat("_Glossiness", .12f);
            EditorUtility.SetDirty(material);
        }
        private static void Build(bool development)
        {
            Validate();
            PrepareMaterials();
            var previous = EditorSceneManager.GetSceneManagerSetup();
            if (!Application.isBatchMode)
            {
                foreach (var scene in previous)
                    if (string.IsNullOrEmpty(scene.path)) throw new InvalidOperationException("Save open scenes before building.");
                for (int i = 0; i < UnityEngine.SceneManagement.SceneManager.sceneCount; i++)
                    if (UnityEngine.SceneManagement.SceneManager.GetSceneAt(i).isDirty) throw new InvalidOperationException("Save scene changes before building.");
            }
            PlayerSettings.productName = BrandTheme.Title;
            PlayerSettings.companyName = "CodenameCheweenie";
            PlayerSettings.bundleVersion = "0.3.1";
            PlayerSettings.defaultScreenWidth = 1280; PlayerSettings.defaultScreenHeight = 720;
            PlayerSettings.fullScreenMode = FullScreenMode.Windowed; PlayerSettings.resizableWindow = true;
            PlayerSettings.SetScriptingBackend(NamedBuildTarget.Standalone, ScriptingImplementation.Mono2x);
            try
            {
                System.IO.Directory.CreateDirectory("Assets/Generated");
                var scene = EditorSceneManager.NewScene(NewSceneSetup.EmptyScene, NewSceneMode.Single);
                EditorSceneManager.SaveScene(scene, "Assets/Generated/Bootstrap.unity");
                string output = development ? "builds/Validation/Cheweenie-validation.exe" : "builds/Windows/Cheweenie.exe";
                System.IO.Directory.CreateDirectory(System.IO.Path.GetDirectoryName(output));
                var report = BuildPipeline.BuildPlayer(new BuildPlayerOptions
                {
                    scenes = new[] { "Assets/Generated/Bootstrap.unity" }, locationPathName = output,
                    target = BuildTarget.StandaloneWindows64,
                    options = BuildOptions.CompressWithLz4HC | (development ? BuildOptions.Development : BuildOptions.None)
                });
                if (report.summary.result != BuildResult.Succeeded) throw new InvalidOperationException("Build failed: " + report.summary.result);
                Debug.Log("Windows build succeeded: " + System.IO.Path.GetFullPath(output) + " (" + report.summary.totalSize + " bytes)");
            }
            finally { if (!Application.isBatchMode) EditorSceneManager.RestoreSceneManagerSetup(previous); }
        }
    }
}
