using System.Collections;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.TestTools;
using UnityEngine.SceneManagement;

namespace CodenameCheweenie.Tests
{
    public sealed class WorldSmokeTests
    {
        [UnityTest] public IEnumerator BootstrapPhysicsQuestAndCompanions()
        {
            GameSave.PersistenceEnabled = false;
            CheweenieBootstrap.Build(); CheweenieBootstrap.Build();
            yield return null;
            Assert.AreEqual(1, Object.FindObjectsByType<MartyController>(FindObjectsSortMode.None).Length);
            var dog = Object.FindFirstObjectByType<MartyController>();
            var quest = Object.FindFirstObjectByType<QuestDirector>();
            var backyard = Object.FindFirstObjectByType<BackyardDirector>();
            var body = dog.GetComponent<Rigidbody>();
            var frontEnd = Object.FindFirstObjectByType<FrontEndMenu>();
            var settings = Object.FindFirstObjectByType<CozySettings>();
            Assert.IsTrue(frontEnd.ShowingTitle);
            Assert.AreEqual(0, Time.timeScale, "Title must freeze gameplay");
            Assert.IsTrue(dog.GetComponent<MartyInput>().Blocked);
            Assert.IsNotNull(Resources.Load<Texture2D>("Branding/Cover"), "Title cover must be bundled as a Resource");
            var catHomes = new System.Collections.Generic.Dictionary<CatCompanion, Vector3>();
            foreach (var cat in Object.FindObjectsByType<CatCompanion>(FindObjectsSortMode.None)) catHomes.Add(cat, cat.transform.position);
            settings.OpenFromTitle(); yield return null;
            Assert.IsTrue(settings.IsOpen);
            settings.Close(); yield return null;
            Assert.IsTrue(frontEnd.ShowingTitle);
            Assert.AreEqual(0, Time.timeScale, "Closing title settings must not start gameplay");
            Assert.IsTrue(dog.GetComponent<MartyInput>().Blocked);
            frontEnd.StartAdventure(false);
            Assert.IsFalse(frontEnd.ShowingTitle);
            Assert.AreEqual(1, Time.timeScale);
            quest.Restore(0);
            backyard.Restore(0);
            foreach (var cat in Object.FindObjectsByType<CatCompanion>(FindObjectsSortMode.None)) cat.Restore(0, false);
            for (int i = 0; i < 60; i++) yield return new WaitForFixedUpdate();
            Assert.Greater(body.position.y, .6f, "Dog must remain above the floor");
            Assert.Less(body.position.y, 1.2f, "Dog must settle onto the floor");
            var targets = Object.FindObjectsByType<InteractableTarget>(FindObjectsSortMode.None);
            InteractableTarget ribbon = null;
            foreach (var target in targets) if (target.backyardClue == BackyardClue.Ribbon) ribbon = target;
            Assert.IsNotNull(ribbon);
            body.position = ribbon.transform.position + new Vector3(0, .3f, -1.5f); Physics.SyncTransforms();
            quest.Activate(ribbon);
            Assert.AreEqual(0, backyard.State, "Backyard discoveries remain locked before the home mission");
            for (int stage = 0; stage < 4; stage++)
            {
                InteractableTarget target = null;
                foreach (var t in targets) if (t.stage == stage) target = t;
                Assert.IsNotNull(target);
                body.position = target.transform.position + new Vector3(0, .3f, -1.5f);
                Physics.SyncTransforms();
                if (stage == 3)
                {
                    quest.Activate(target); Assert.AreEqual(3, quest.CurrentStage, "Sniff must not finish the fort");
                }
                quest.Activate(target, stage == 3 ? InteractionKind.Cozy : InteractionKind.Sniff);
                Assert.AreEqual(stage + 1, quest.CurrentStage);
            }
            foreach (var cat in Object.FindObjectsByType<CatCompanion>(FindObjectsSortMode.None))
            { Assert.IsTrue(cat.CompanionQuestComplete); Assert.GreaterOrEqual(cat.Friendship, 2); }
            Assert.IsTrue(backyard.Unlocked);
            body.position = ribbon.transform.position + new Vector3(0, .3f, -1.5f); Physics.SyncTransforms();
            quest.Activate(ribbon); Assert.AreEqual(0, backyard.State, "Ribbon needs all three routes");
            foreach (var clue in new[] { BackyardClue.PawPassage, BackyardClue.Lookout, BackyardClue.ScentTrail })
            {
                InteractableTarget target = null;
                foreach (var t in targets) if (t.backyardClue == clue) target = t;
                Assert.IsNotNull(target);
                if (clue == BackyardClue.PawPassage)
                {
                    body.position = new Vector3(8.7f, .7f, -13); Physics.SyncTransforms();
                    Assert.IsFalse(target.CanReach(dog.transform, dog.tuning.interactionRange), "The hedge wall blocks interaction");
                    quest.Activate(target); Assert.AreEqual(0, backyard.State);
                }
                body.position = target.transform.position + new Vector3(0, .3f, 1.5f); Physics.SyncTransforms();
                quest.Activate(target); Assert.IsTrue(backyard.Has(clue));
            }
            body.position = ribbon.transform.position + new Vector3(0, .3f, -1.5f); Physics.SyncTransforms();
            quest.Activate(ribbon); Assert.AreEqual(15, backyard.State);
            Assert.AreEqual(15, GameSave.Data.backyardState, "Discovery is captured in the save data");
            foreach (var cat in Object.FindObjectsByType<CatCompanion>(FindObjectsSortMode.None))
            {
                if (cat.energetic) continue;
                body.position = cat.transform.position + new Vector3(0, .3f, -1.5f); Physics.SyncTransforms();
                quest.Activate(cat.GetComponent<InteractableTarget>());
            }
            Assert.IsTrue(backyard.Complete);
            Assert.AreEqual(31, GameSave.Data.backyardState);
            Assert.AreEqual(4, quest.CurrentStage, "M6 preserves the initial quest's completed state");
            Assert.IsNotNull(GameObject.Find("Accessible loft ramp"));
            Assert.IsNotNull(Object.FindFirstObjectByType<BetweenSpot>());
            GameSave.Data.volume = .42f;
            foreach (var pair in catHomes) pair.Key.GetComponent<Rigidbody>().position += Vector3.right * 2;
            GameSave.NewGame();
            Assert.AreEqual(0, quest.CurrentStage);
            Assert.AreEqual(0, backyard.State);
            Assert.AreEqual(0, GameSave.Data.backyardState);
            Assert.AreEqual(.42f, GameSave.Data.volume, "New Adventure must preserve settings");
            Assert.Less(Vector3.Distance(body.position, new Vector3(0, .95f, -1)), .01f);
            foreach (var pair in catHomes)
            {
                Assert.Less(Vector3.Distance(pair.Key.GetComponent<Rigidbody>().position, pair.Value), .01f);
                Assert.IsFalse(pair.Key.CompanionQuestComplete);
                Assert.AreEqual(0, pair.Key.Friendship);
                pair.Key.Befriend(); Assert.AreEqual(1, pair.Key.Friendship, "A new adventure clears greeting cooldowns");
            }
            settings.OpenFromTitle(); yield return null;
            settings.ReturnToTitle(); yield return null;
            Assert.IsFalse(settings.IsOpen);
            Assert.IsTrue(frontEnd.ShowingTitle);
            Assert.AreEqual(0, Time.timeScale);
            frontEnd.StartAdventure(false);
            Assert.AreEqual(1, Time.timeScale, "Returning to title must not lose the gameplay time scale");
            LogAssert.NoUnexpectedReceived();
        }
        [UnityTearDown] public IEnumerator Cleanup()
        {
            // All bootstrap objects belong to this test scene. Do not overwrite the player's save.
            var quest = Object.FindFirstObjectByType<QuestDirector>();
            if (quest) Object.Destroy(quest.gameObject);
            yield return null;
            GameSave.Reload(); GameSave.PersistenceEnabled = true;
        }
    }
}
