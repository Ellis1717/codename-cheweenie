using System.Collections;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.TestTools;

namespace CodenameCheweenie.Tests
{
    public sealed class ReleaseReadinessTests
    {
        private GameObject originalCamera, originalSun;
        [UnityTest] public IEnumerator DefaultScenePresentationIsSuspendedAndRestoredWithoutDeletingObjects()
        {
            GameSave.PersistenceEnabled = false;
            var previous = Object.FindFirstObjectByType<QuestDirector>(); if (previous) Object.Destroy(previous.gameObject);
            yield return null;
            originalCamera = new GameObject("Existing scene camera"); var camera = originalCamera.AddComponent<Camera>(); camera.tag = "MainCamera";
            var listener = originalCamera.AddComponent<AudioListener>(); originalCamera.transform.position = new Vector3(3, 2, -10);
            originalSun = new GameObject("Existing scene sun"); var sun = originalSun.AddComponent<Light>(); sun.type = LightType.Directional;
            Color ambient = RenderSettings.ambientLight;
            CheweenieBootstrap.Build(); yield return null;
            Assert.IsFalse(camera.enabled); Assert.IsFalse(listener.enabled); Assert.IsFalse(sun.enabled);
            Assert.AreEqual(1, System.Array.FindAll(Object.FindObjectsByType<Camera>(FindObjectsSortMode.None), item => item.enabled).Length);
            Assert.AreEqual(1, System.Array.FindAll(Object.FindObjectsByType<AudioListener>(FindObjectsSortMode.None), item => item.enabled).Length);
            Assert.AreEqual("Follow Camera", Camera.main.name);
            Object.Destroy(Object.FindFirstObjectByType<QuestDirector>().gameObject); yield return null;
            Assert.IsTrue(camera.enabled); Assert.IsTrue(listener.enabled); Assert.IsTrue(sun.enabled);
            Assert.AreEqual(new Vector3(3, 2, -10), originalCamera.transform.position); Assert.AreEqual(ambient, RenderSettings.ambientLight);
            LogAssert.NoUnexpectedReceived();
        }
        [UnityTest] public IEnumerator ReturnHomeKeepsAllProgressAndClosesThePauseMenu()
        {
            GameSave.PersistenceEnabled = false; CheweenieBootstrap.Build(); yield return null;
            var quest = Object.FindFirstObjectByType<QuestDirector>(); var settings = quest.GetComponent<CozySettings>();
            quest.GetComponent<FrontEndMenu>().StartAdventure(true); yield return null;
            quest.Restore(4); quest.GetComponent<BackyardDirector>().Restore(31);
            var neighbourhood = quest.GetComponent<NeighbourhoodDirector>(); int state = MappedNeighbourhood.Current != null && MappedNeighbourhood.Current.IncludesSchool ? 31 : 15;
            neighbourhood.Restore(state);
            var cats = Object.FindObjectsByType<CatCompanion>(FindObjectsSortMode.None); foreach (var cat in cats) cat.Restore(3, true);
            GameSave.Data.volume = .42f; GameSave.Data.musicVolume = .27f; GameSave.Data.cameraScale = 1;
            var dog = Object.FindFirstObjectByType<MartyController>(); var body = dog.GetComponent<Rigidbody>();
            body.position = new Vector3(60, .95f, -25); dog.transform.position = body.position; Physics.SyncTransforms();
            settings.OpenFromTitle(); yield return null; Assert.AreEqual(0, Time.timeScale);
            settings.ReturnHome(); yield return null;
            Assert.IsFalse(settings.IsOpen); Assert.IsFalse(dog.GetComponent<MartyInput>().Blocked); Assert.AreEqual(1, Time.timeScale);
            Assert.Less(Vector3.Distance(body.position, new Vector3(0, .95f, -1)), .15f);
            Assert.Less(Vector3.Distance(Camera.main.transform.position, dog.transform.position), 12);
            Assert.AreEqual(4, quest.CurrentStage); Assert.AreEqual(31, quest.GetComponent<BackyardDirector>().State); Assert.AreEqual(state, neighbourhood.State);
            foreach (var cat in cats) { Assert.AreEqual(3, cat.Friendship); Assert.IsTrue(cat.CompanionQuestComplete); }
            Assert.AreEqual(.42f, GameSave.Data.volume); Assert.AreEqual(.27f, GameSave.Data.musicVolume);
            LogAssert.NoUnexpectedReceived();
        }
        [UnityTearDown] public IEnumerator Cleanup()
        {
            var quest = Object.FindFirstObjectByType<QuestDirector>(); if (quest) Object.Destroy(quest.gameObject);
            yield return null; if (originalCamera) Object.Destroy(originalCamera); if (originalSun) Object.Destroy(originalSun);
            yield return null; GameSave.Reload(); GameSave.PersistenceEnabled = true; Time.timeScale = 1;
        }
    }
}
