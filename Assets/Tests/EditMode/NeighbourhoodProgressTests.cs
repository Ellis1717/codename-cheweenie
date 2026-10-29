using NUnit.Framework;

namespace CodenameCheweenie.Tests
{
    public sealed class NeighbourhoodProgressTests
    {
        [TestCase(1, 2, 4)] [TestCase(1, 4, 2)] [TestCase(2, 1, 4)]
        [TestCase(2, 4, 1)] [TestCase(4, 1, 2)] [TestCase(4, 2, 1)]
        public void VisitEveryOrderThenReturnHome(int a, int b, int c)
        {
            var progress = new NeighbourhoodProgress();
            Assert.IsFalse(progress.Visit(NeighbourhoodStop.Home));
            foreach (int stop in new[] { a, b, c })
            { Assert.IsTrue(progress.Visit((NeighbourhoodStop)stop)); Assert.IsFalse(progress.Visit((NeighbourhoodStop)stop)); }
            Assert.AreEqual(3, progress.Visits); Assert.IsFalse(progress.Complete);
            Assert.IsTrue(progress.Visit(NeighbourhoodStop.Home)); Assert.IsTrue(progress.Complete);
            Assert.AreEqual(15, new NeighbourhoodProgress(progress.State).State);
            Assert.IsFalse(progress.Visit(NeighbourhoodStop.Home));
        }
        [TestCase(-1, 0)] [TestCase(0, 0)] [TestCase(8, 0)] [TestCase(9, 1)]
        [TestCase(7, 7)] [TestCase(15, 15)] [TestCase(1027, 3)]
        public void RestoresOldPartialAndInvalidSaves(int saved, int expected)
        { Assert.AreEqual(expected, new NeighbourhoodProgress(saved).State); }
        [Test] public void RejectsUnknownAndCombinedStops()
        {
            var progress = new NeighbourhoodProgress();
            foreach (int invalid in new[] { -1, 0, 3, 16, 1024 }) Assert.IsFalse(progress.Visit((NeighbourhoodStop)invalid));
            Assert.IsFalse(progress.Has(NeighbourhoodStop.None)); Assert.AreEqual(0, progress.State);
        }
        [Test] public void OlderSavesKeepExistingProgressAndStartANewWalk()
        {
            var saved = UnityEngine.JsonUtility.FromJson<SaveData>("{\"version\":1,\"stage\":4,\"backyardState\":31,\"volume\":0.42}");
            Assert.AreEqual(0, saved.neighbourhoodState); Assert.AreEqual(4, saved.stage);
            Assert.AreEqual(31, saved.backyardState); Assert.AreEqual(.42f, saved.volume);
        }
        [Test] public void NewProgressSurvivesJsonSerialization()
        {
            var saved = new SaveData { neighbourhoodState = 15, stage = 4, backyardState = 31 };
            var restored = UnityEngine.JsonUtility.FromJson<SaveData>(UnityEngine.JsonUtility.ToJson(saved));
            Assert.AreEqual(15, restored.neighbourhoodState); Assert.AreEqual(31, restored.backyardState);
        }
        [Test] public void ExtendedWalkKeepsOldStampsButRequiresSchoolBeforeReturning()
        {
            var progress = new NeighbourhoodProgress(15, true);
            Assert.AreEqual(7, progress.State); Assert.AreEqual(3, progress.Visits); Assert.IsFalse(progress.Complete);
            Assert.IsFalse(progress.Visit(NeighbourhoodStop.Home));
            Assert.IsTrue(progress.Visit(NeighbourhoodStop.School)); Assert.AreEqual(4, progress.Visits);
            Assert.IsTrue(progress.Visit(NeighbourhoodStop.Home)); Assert.AreEqual(31, progress.State);
            Assert.IsTrue(new NeighbourhoodProgress(31, true).Complete);
        }
    }
}
