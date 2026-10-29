using System.Collections;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.TestTools;

namespace CodenameCheweenie.Tests
{
    public sealed class CatVisualTests
    {
        [UnityTest] public IEnumerator PhotoGuidedVisualsKeepTheOriginalCompanionPhysics()
        {
            GameSave.PersistenceEnabled = false; CheweenieBootstrap.Build(); yield return null;
            var cats = Object.FindObjectsByType<CatVisual>(FindObjectsSortMode.None); Assert.AreEqual(2, cats.Length);
            foreach (var cat in cats)
            {
                var companion = cat.GetComponent<CatCompanion>(); Assert.IsNotNull(companion);
                Assert.AreEqual(companion.energetic ? CatLook.Lakitu : CatLook.Rio, cat.look);
                Assert.AreEqual(1, cat.GetComponentsInChildren<Rigidbody>().Length);
                Assert.IsTrue(cat.GetComponent<Rigidbody>().isKinematic);
                foreach (var collider in cat.GetComponentsInChildren<Collider>()) if (collider.transform != cat.transform) Assert.IsFalse(collider.enabled);
                var head = cat.transform.Find(cat.look + " | reference-based cat rig/Reference-patterned head"); Assert.IsNotNull(head);
                var face = head.GetComponent<Renderer>().sharedMaterial; Assert.IsNotNull(face.shader); Assert.IsNotNull(face.mainTexture);
                Assert.AreEqual(256, face.mainTexture.width); Assert.AreEqual(128, face.mainTexture.height);
                Assert.AreEqual(4, System.Array.FindAll(cat.GetComponentsInChildren<Transform>(), part => part.name.StartsWith("Paw ")).Length);
            }
            LogAssert.NoUnexpectedReceived();
        }
        [UnityTearDown] public IEnumerator Cleanup()
        {
            var quest = Object.FindFirstObjectByType<QuestDirector>(); if (quest) Object.Destroy(quest.gameObject);
            yield return null; GameSave.Reload(); GameSave.PersistenceEnabled = true; Time.timeScale = 1;
        }
    }
}
