using NUnit.Framework;

namespace CodenameCheweenie.Tests
{
    public sealed class QuestProgressTests
    {
        [Test] public void MissionsRequireOrderAndCozyAtFort()
        {
            var progress = new QuestProgress();
            Assert.IsFalse(progress.TryAdvance(1, InteractionKind.Sniff));
            for (int stage = 0; stage < 3; stage++)
            {
                Assert.IsTrue(progress.TryAdvance(stage, InteractionKind.Sniff));
                Assert.IsFalse(progress.TryAdvance(stage, InteractionKind.Sniff));
            }
            Assert.IsFalse(progress.TryAdvance(3, InteractionKind.Sniff));
            Assert.IsTrue(progress.TryAdvance(3, InteractionKind.Cozy));
            Assert.AreEqual(4, progress.Stage);
            Assert.IsFalse(progress.TryAdvance(4, InteractionKind.Cozy));
        }
        [TestCase(-8, 0)] [TestCase(2, 2)] [TestCase(99, 4)]
        public void SavedStagesAreClamped(int saved, int expected)
        {
            Assert.AreEqual(expected, new QuestProgress(saved).Stage);
        }
    }
}
