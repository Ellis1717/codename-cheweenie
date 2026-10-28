using System;
using CodenameCheweenie;

static class BackyardChecks
{
    public static void Run(Action<bool> check)
    {
        var p = new BackyardProgress();
        check(!p.TryDiscover(BackyardClue.ScentTrail, false));
        check(!p.TryDiscover(BackyardClue.Ribbon, true));
        check(!p.TryDiscover(BackyardClue.Returned, true));
        check(!p.TryDiscover(BackyardClue.None, true));
        check(!p.TryDiscover((BackyardClue)3, true));
        int[][] orders = { new[] { 1, 2, 4 }, new[] { 1, 4, 2 }, new[] { 2, 1, 4 }, new[] { 2, 4, 1 }, new[] { 4, 1, 2 }, new[] { 4, 2, 1 } };
        foreach (var order in orders)
        {
            p = new BackyardProgress();
            foreach (int route in order) check(p.TryDiscover((BackyardClue)route, true));
            check(p.RouteCount == 3);
            check(!p.TryDiscover(BackyardClue.Returned, true));
            check(p.TryDiscover(BackyardClue.Ribbon, true));
            check(!p.TryDiscover(BackyardClue.Returned, false));
            check(p.TryDiscover(BackyardClue.Returned, true));
            check(p.Complete && new BackyardProgress(p.State).State == 31);
            check(!p.TryDiscover(BackyardClue.Returned, true));
        }
        check(new BackyardProgress(-1).State == 0);
        check(new BackyardProgress(8).State == 0);
        check(new BackyardProgress(16).State == 0);
        check(new BackyardProgress(23).State == 7);
        check(new BackyardProgress(15).State == 15);
        check(new BackyardProgress(1027).State == 3);
        p = new BackyardProgress(3);
        check(p.RouteCount == 2);
        check(!p.TryDiscover(BackyardClue.ScentTrail, true));
        check(!p.TryDiscover((BackyardClue)32, true));
        check(p.TryDiscover(BackyardClue.Lookout, true));
        check(p.Available(BackyardClue.Ribbon, true));
    }
}
