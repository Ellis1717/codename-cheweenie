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
            Quaternion rest=rig.legs[0].localRotation;rig.Animate(5,false,false);yield return new WaitForSeconds(.1f);rig.Animate(5,false,false);
            Assert.Greater(Quaternion.Angle(rest,rig.legs[0].localRotation),.01f);
            rig.Animate(0,true,false);Assert.Greater(Quaternion.Angle(Quaternion.identity,rig.knees[0].localRotation),40);
            dog.ResetToHome();Assert.AreEqual(0,quest.CurrentStage);LogAssert.NoUnexpectedReceived();
        }
        [UnityTearDown] public IEnumerator Cleanup()
        {
            var quest=Object.FindFirstObjectByType<QuestDirector>();if(quest)Object.Destroy(quest.gameObject);
            yield return null;GameSave.Reload();GameSave.PersistenceEnabled=true;Time.timeScale=1;
        }
    }
}
