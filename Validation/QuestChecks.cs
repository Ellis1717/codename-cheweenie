using System;
using CodenameCheweenie;

static class QuestChecks
{
    static int checks;
    static void Check(bool value) { checks++; if (!value) throw new Exception("Quest check " + checks + " failed"); }
    static void Main()
    {
        var q = new QuestProgress();
        Check(!q.TryAdvance(1, InteractionKind.Sniff));
        for (int i = 0; i < 3; i++)
        {
            Check(q.TryAdvance(i, InteractionKind.Sniff));
            Check(!q.TryAdvance(i, InteractionKind.Sniff));
        }
        Check(!q.TryAdvance(3, InteractionKind.Sniff));
        Check(q.TryAdvance(3, InteractionKind.Cozy));
        Check(!q.TryAdvance(4, InteractionKind.Cozy));
        Check(new QuestProgress(-1).Stage == 0);
        Check(new QuestProgress(99).Stage == 4);
        Check(new QuestProgress(2).Stage == 2);
        Console.WriteLine("PASS: " + checks + " pure quest checks (outside Unity)");
    }
}
