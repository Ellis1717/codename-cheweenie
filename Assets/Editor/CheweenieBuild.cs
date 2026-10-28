using System;
using UnityEditor;
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
                var serialized = new SerializedObject(settings[0]);
                var handler = serialized.FindProperty("activeInputHandler");
                if (handler != null && handler.intValue == 1)
                    throw new InvalidOperationException("Set Active Input Handling to Input Manager (Old) or Both, then restart Unity.");
            }
            if (!System.IO.File.Exists("ProjectSettings/InputManager.asset")) throw new InvalidOperationException("Missing joystick axes.");
            string input = System.IO.File.ReadAllText("ProjectSettings/InputManager.asset");
            if (!input.Contains("m_Name: DogStickX") || !input.Contains("m_Name: DogStickY"))
                throw new InvalidOperationException("Missing DogStickX / DogStickY axes.");
            Debug.Log("Cheweenie configuration checks passed. Run EditMode and PlayMode tests, then verify physical controllers.");
        }
        [MenuItem("Cheweenie/Build Windows prototype")]
        public static void BuildWindows()
        {
            Validate();
            var previousSetup = EditorSceneManager.GetSceneManagerSetup();
            foreach (var scene in previousSetup)
                if (string.IsNullOrEmpty(scene.path)) throw new InvalidOperationException("Save open scenes before building.");
            for (int i = 0; i < UnityEngine.SceneManagement.SceneManager.sceneCount; i++)
                if (UnityEngine.SceneManagement.SceneManager.GetSceneAt(i).isDirty) throw new InvalidOperationException("Save unsaved scene changes before building.");
            try
            {
                System.IO.Directory.CreateDirectory("Assets/Generated");
                var empty = EditorSceneManager.NewScene(NewSceneSetup.EmptyScene, NewSceneMode.Single);
                EditorSceneManager.SaveScene(empty, "Assets/Generated/Bootstrap.unity");
                var report = BuildPipeline.BuildPlayer(new BuildPlayerOptions
                {
                    scenes = new[] { "Assets/Generated/Bootstrap.unity" }, locationPathName = "builds/Windows/Cheweenie.exe",
                    target = BuildTarget.StandaloneWindows64, options = BuildOptions.Development
                });
                if (report.summary.result != BuildResult.Succeeded) throw new InvalidOperationException("Build failed: " + report.summary.result);
            }
            finally { EditorSceneManager.RestoreSceneManagerSetup(previousSetup); }
        }
    }
}
