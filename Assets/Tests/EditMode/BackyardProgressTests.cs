using NUnit.Framework;

namespace CodenameCheweenie.Tests
{
    public sealed class BackyardProgressTests
    {
        [Test] public void HomeMissionMustFinishBeforeDiscovery()
        {
            var progress = new BackyardProgress();
            Assert.IsFalse(progress.TryDiscover(BackyardClue.ScentTrail, false));
            Assert.IsFalse(progress.TryDiscover(BackyardClue.Ribbon, true));
            Assert.IsFalse(progress.TryDiscover(BackyardClue.Returned, true));
            Assert.AreEqual(0, progress.State);
        }
        [TestCase(1, 2, 4)] [TestCase(1, 4, 2)] [TestCase(2, 1, 4)]
        [TestCase(2, 4, 1)] [TestCase(4, 1, 2)] [TestCase(4, 2, 1)]
        public void RoutesWorkInAnyOrderAndRequireRibbonBeforeReturn(int a, int b, int c)
        {
            var progress = new BackyardProgress();
            foreach (int route in new[] { a, b, c }) Assert.IsTrue(progress.TryDiscover((BackyardClue)route, true));
            Assert.AreEqual(3, progress.RouteCount);
            Assert.IsFalse(progress.TryDiscover(BackyardClue.Returned, true));
            Assert.IsTrue(progress.TryDiscover(BackyardClue.Ribbon, true));
            Assert.IsFalse(progress.TryDiscover(BackyardClue.Returned, false));
            Assert.IsTrue(progress.TryDiscover(BackyardClue.Returned, true));
            Assert.IsTrue(progress.Complete);
            Assert.IsFalse(progress.TryDiscover(BackyardClue.Returned, true));
            Assert.AreEqual(31, new BackyardProgress(progress.State).State);
        }
        [TestCase(-1, 0)] [TestCase(0, 0)] [TestCase(8, 0)] [TestCase(16, 0)]
        [TestCase(23, 7)] [TestCase(15, 15)] [TestCase(31, 31)] [TestCase(1027, 3)]
        public void RestoreDropsUnknownAndImpossibleProgress(int saved, int expected)
        { Assert.AreEqual(expected, new BackyardProgress(saved).State); }
        [Test] public void PartialSaveDoesNotLoseDiscoveriesOrAllowDuplicates()
        {
            var progress = new BackyardProgress(3);
            Assert.AreEqual(2, progress.RouteCount);
            Assert.IsFalse(progress.TryDiscover(BackyardClue.ScentTrail, true));
            Assert.IsFalse(progress.TryDiscover(BackyardClue.None, true));
            Assert.IsFalse(progress.TryDiscover((BackyardClue)3, true));
            Assert.IsFalse(progress.TryDiscover((BackyardClue)32, true));
            Assert.IsTrue(progress.TryDiscover(BackyardClue.Lookout, true));
            Assert.IsTrue(progress.Available(BackyardClue.Ribbon, true));
        }
    }
}
