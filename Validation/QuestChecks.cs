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
        int questChecks = checks;
        int[] defaults = { 1, 2, 3 };
        Func<int, bool> allowed = key => key > 0 && key < 10;
        var map = BindingMap.Restore(defaults, new[] { 2, 2, 99 }, allowed);
        Check(map[0] == 2 && map[1] == 1 && map[2] == 3);
        Check(defaults[0] == 1 && defaults[1] == 2 && defaults[2] == 3);
        map = BindingMap.Restore(defaults, new[] { 2, 3, 1 }, allowed);
        Check(map[0] == 2 && map[1] == 3 && map[2] == 1);
        map = BindingMap.Restore(defaults, null, allowed);
        Check(map != defaults && map[0] == 1 && map[1] == 2 && map[2] == 3);
        map = BindingMap.Restore(defaults, new[] { 8 }, allowed);
        Check(map[0] == 1 && map[1] == 2 && map[2] == 3);
        Check(BindingMap.Assign(map, 0, 3, allowed));
        Check(map[0] == 3 && map[1] == 2 && map[2] == 1);
        Check(!BindingMap.Assign(map, 0, 99, allowed));
        Check(!BindingMap.Assign(map, -1, 4, allowed));
        Check(!BindingMap.Assign(map, 3, 4, allowed));
        Check(!BindingMap.Assign(null, 0, 4, allowed));
        Check(map[0] == 3 && map[1] == 2 && map[2] == 1);
        var gate = new InputReleaseGate();
        Check(gate.CanRead(0, false, true));
        gate.Suppress(10);
        Check(!gate.CanRead(10, false, false));
        Check(!gate.CanRead(11, false, true));
        Check(!gate.CanRead(12, false, true));
        Check(!gate.CanRead(13, true, false));
        Check(gate.WaitingForRelease);
        Check(gate.CanRead(14, false, false));
        Check(!gate.WaitingForRelease);
        Check(gate.CanRead(15, false, true));
        gate.Suppress(16);
        Check(!gate.CanRead(17, false, true));
        Check(gate.CanRead(18, false, false));
        Console.WriteLine("PASS: " + checks + " checks (" + questChecks + " quest, " + (checks - questChecks) + " bindings/input gate), outside Unity");
    }
}
