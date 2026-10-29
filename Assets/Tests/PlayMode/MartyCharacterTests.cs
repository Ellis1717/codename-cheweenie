using System.Collections;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.TestTools;

namespace CodenameCheweenie.Tests
{
    public sealed class MartyCharacterTests
    {
        [UnityTest] public IEnumerator BootstrapUsesTheSavedCharacterWithoutReplacingGameplayPhysics()
        {
            GameSave.PersistenceEnabled=false;CheweenieBootstrap.Build();yield return null;
            var dog=Object.FindFirstObjectByType<MartyController>();var visual=dog.GetComponent<CanineVisual>();
            Assert.IsTrue(visual.UsesAuthoredModel);var rig=dog.GetComponentInChildren<MartyModelRig>();Assert.IsNotNull(rig);
            Assert.AreEqual(1,dog.GetComponentsInChildren<Rigidbody>().Length);Assert.AreEqual(1,dog.GetComponentsInChildren<Collider>().Length);
            var quest=Object.FindFirstObjectByType<QuestDirector>();quest.GetComponent<FrontEndMenu>().StartAdventure(true);yield return null;
            float cycle=rig.Gait.Cycle;rig.Animate(5,false,false);Assert.AreEqual(cycle,rig.Gait.Cycle,"Stationary geometry must not march from a requested speed alone.");
            dog.enabled=false;for(int i=0;i<24;i++){rig.Animate(0,true,false);yield return null;}
            Assert.Greater(Quaternion.Angle(Quaternion.identity,rig.knees[0].localRotation),40);
            dog.ResetToHome();Assert.AreEqual(0,quest.CurrentStage);LogAssert.NoUnexpectedReceived();
        }
        [UnityTearDown] public IEnumerator Cleanup()
        {
            var quest=Object.FindFirstObjectByType<QuestDirector>();if(quest)Object.Destroy(quest.gameObject);
            yield return null;GameSave.Reload();GameSave.PersistenceEnabled=true;Time.timeScale=1;
        }
    }
}
