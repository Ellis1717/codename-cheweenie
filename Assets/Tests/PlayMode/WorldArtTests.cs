using System.Collections;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.TestTools;

namespace CodenameCheweenie.Tests
{
    public sealed class WorldArtTests
    {
        private GameObject userObject;
        [UnityTest] public IEnumerator RetainedArtPreservesExistingCollisionAndUserSceneObjects()
        {
            GameSave.PersistenceEnabled=false;var previous=Object.FindFirstObjectByType<QuestDirector>();if(previous)Object.Destroy(previous.gameObject);yield return null;
            userObject=GameObject.CreatePrimitive(PrimitiveType.Cube);userObject.name="User scene object";userObject.transform.position=new Vector3(1000,2,0);var material=userObject.GetComponent<Renderer>().sharedMaterial;
            CheweenieBootstrap.Build();yield return null;
            var art=Object.FindFirstObjectByType<WorldArt>();Assert.IsTrue(art.Ready);Assert.Greater(art.PropCount,70);
            Assert.AreSame(material,userObject.GetComponent<Renderer>().sharedMaterial);Assert.IsTrue(userObject.GetComponent<Renderer>().enabled);
            var sofa=GameObject.Find("Sofa");Assert.IsTrue(sofa.GetComponent<Collider>().enabled);Assert.IsFalse(sofa.GetComponent<Renderer>().enabled);Assert.AreEqual(new Vector3(2.5f,1.1f,1),sofa.transform.localScale);
            var visuals=art.transform.Find("Retained cozy scenery | visual layer");Assert.IsNotNull(visuals);foreach(var collider in visuals.GetComponentsInChildren<Collider>())Assert.IsFalse(collider.enabled);Assert.AreEqual(0,visuals.GetComponentsInChildren<Rigidbody>().Length);
            foreach(var cat in Object.FindObjectsByType<CatVisual>(FindObjectsSortMode.None)){Assert.IsTrue(cat.UsesRetainedModel);Assert.AreEqual(1,cat.GetComponentsInChildren<Rigidbody>().Length);}
            LogAssert.NoUnexpectedReceived();
        }
        [UnityTest] public IEnumerator SceneryLightingRestoresTheOriginalEnvironmentOnTeardown()
        {
            GameSave.PersistenceEnabled=false;var previous=Object.FindFirstObjectByType<QuestDirector>();if(previous)Object.Destroy(previous.gameObject);yield return null;
            bool fog=RenderSettings.fog;var mode=RenderSettings.ambientMode;Color sky=RenderSettings.ambientSkyColor;float distance=QualitySettings.shadowDistance,end=RenderSettings.fogEndDistance;
            CheweenieBootstrap.Build();yield return null;Assert.IsTrue(RenderSettings.fog);Assert.AreEqual(300,RenderSettings.fogEndDistance);Assert.AreEqual(65,QualitySettings.shadowDistance);
            Object.Destroy(Object.FindFirstObjectByType<QuestDirector>().gameObject);yield return null;
            Assert.AreEqual(fog,RenderSettings.fog);Assert.AreEqual(mode,RenderSettings.ambientMode);Assert.AreEqual(sky,RenderSettings.ambientSkyColor);Assert.AreEqual(distance,QualitySettings.shadowDistance);Assert.AreEqual(end,RenderSettings.fogEndDistance);LogAssert.NoUnexpectedReceived();
        }
        [UnityTearDown] public IEnumerator Cleanup()
        {
            var quest=Object.FindFirstObjectByType<QuestDirector>();if(quest)Object.Destroy(quest.gameObject);yield return null;if(userObject)Object.Destroy(userObject);yield return null;GameSave.Reload();GameSave.PersistenceEnabled=true;Time.timeScale=1;
        }
    }
}
