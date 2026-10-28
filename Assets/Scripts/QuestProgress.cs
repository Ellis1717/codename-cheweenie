using System;

namespace CodenameCheweenie
{
    public enum InteractionKind { Sniff, Cozy }

    // No Unity dependencies: save restoration and ordering share the same rules.
    public sealed class QuestProgress
    {
        public const int CompletedStage = 4;
        public int Stage { get; private set; }
        public QuestProgress(int stage = 0) { Stage = Math.Max(0, Math.Min(stage, CompletedStage)); }
        public bool TryAdvance(int targetStage, InteractionKind kind)
        {
            if (Stage == CompletedStage || targetStage != Stage) return false;
            if (Stage == 3 && kind != InteractionKind.Cozy) return false;
            Stage++;
            return true;
        }
    }
}
