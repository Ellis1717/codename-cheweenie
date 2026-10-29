using System.Collections;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.TestTools;

namespace CodenameCheweenie.Tests
{
    public sealed class NeighbourhoodTests
    {
        [UnityTest] public IEnumerator WalkIsAvailableBeforeHomeQuestsAndSavesIndependently()
        {
            GameSave.PersistenceEnabled = false; CheweenieBootstrap.Build(); yield return null;
            var quest = Object.FindFirstObjectByType<QuestDirector>();
            var director = quest.GetComponent<NeighbourhoodDirector>();
            var dog = Object.FindFirstObjectByType<MartyController>(); var body = dog.GetComponent<Rigidbody>();
            quest.GetComponent<FrontEndMenu>().StartAdventure(true); yield return null;
            Assert.IsFalse(Physics.Linecast(new Vector3(15, .7f, -13), new Vector3(25, .7f, -13)), "East gate must be open");
            Assert.IsTrue(Physics.Raycast(new Vector3(63, 4, -26), Vector3.down, 5), "Expanded ground supports the walking area");
            foreach (var stop in new[] { NeighbourhoodStop.Mailbox, NeighbourhoodStop.Garden, NeighbourhoodStop.Park, NeighbourhoodStop.Home })
            {
                InteractableTarget target = null;
                foreach (var candidate in InteractableTarget.Active) if (candidate.neighbourhoodStop == stop) target = candidate;
                Assert.IsNotNull(target);
                body.position = target.transform.position + new Vector3(0, .25f, -1);
                dog.transform.position = body.position; Physics.SyncTransforms();
                quest.Activate(target); Assert.IsTrue(director.Has(stop), "Actual quest routing must activate " + stop);
            }
            GameSave.Capture(); Assert.AreEqual(15, GameSave.Data.neighbourhoodState);
            Assert.AreEqual(0, quest.CurrentStage, "Neighbourhood play does not advance home missions");
            director.Restore(GameSave.Data.neighbourhoodState); Assert.IsTrue(director.Complete);
            GameSave.NewGame(); Assert.AreEqual(0, director.State); Assert.AreEqual(0, GameSave.Data.neighbourhoodState);
            LogAssert.NoUnexpectedReceived();
        }
        [UnityTearDown] public IEnumerator Cleanup()
        {
            var quest = Object.FindFirstObjectByType<QuestDirector>(); if (quest) Object.Destroy(quest.gameObject);
            yield return null; GameSave.Reload(); GameSave.PersistenceEnabled = true; Time.timeScale = 1;
        }
    }
}
