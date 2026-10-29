using System.Collections;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.TestTools;
namespace CodenameCheweenie.Tests
{
    public sealed class VisualLabTests
    {
        [UnityTest] public IEnumerator ExperimentalPresentationKeepsRootPhysicsAndRestoresQuality()
        {
            GameSave.PersistenceEnabled=false;int aa=QualitySettings.antiAliasing,lights=QualitySettings.pixelLightCount;
            CheweenieBootstrap.Build();var quest=Object.FindFirstObjectByType<QuestDirector>();var dog=Object.FindFirstObjectByType<MartyController>();LogAssert.Expect(LogType.Log,new System.Text.RegularExpressions.Regex("^VISUAL_LAB_READY "));var lab=quest.gameObject.AddComponent<VisualLab>();lab.Configure(dog.GetComponent<CanineVisual>());
            yield return null;yield return null;yield return new WaitForSecondsRealtime(.2f);
            Assert.IsTrue(lab.Ready);Assert.AreEqual(3,quest.GetComponentsInChildren<VisualLabFur>().Length);Assert.AreEqual(1,dog.GetComponentsInChildren<Rigidbody>().Length);Assert.AreEqual(1,dog.GetComponentsInChildren<Collider>().Length);Assert.IsTrue(dog.GetComponent<CanineVisual>().UsesAuthoredModel);
            Object.Destroy(quest.gameObject);yield return null;Assert.AreEqual(aa,QualitySettings.antiAliasing);Assert.AreEqual(lights,QualitySettings.pixelLightCount);LogAssert.NoUnexpectedReceived();
            GameSave.Reload();GameSave.PersistenceEnabled=true;Time.timeScale=1;
        }
    }
}
