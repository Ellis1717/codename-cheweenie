using System;

namespace CodenameCheweenie
{
    [Flags] public enum BackyardClue { None = 0, ScentTrail = 1, PawPassage = 2, Lookout = 4, Ribbon = 8, Returned = 16 }

    public sealed class BackyardProgress
    {
        private const BackyardClue Routes = BackyardClue.ScentTrail | BackyardClue.PawPassage | BackyardClue.Lookout;
        public BackyardClue Found { get; private set; }
        public int State => (int)Found;
        public int RouteCount => (Has(BackyardClue.ScentTrail) ? 1 : 0) + (Has(BackyardClue.PawPassage) ? 1 : 0) + (Has(BackyardClue.Lookout) ? 1 : 0);
        public bool Complete => Has(BackyardClue.Returned);
        public BackyardProgress(int saved = 0)
        {
            Found = (BackyardClue)((saved < 0 ? 0 : saved) & 31);
            if ((Found & Routes) != Routes) Found &= ~(BackyardClue.Ribbon | BackyardClue.Returned);
            if (!Has(BackyardClue.Ribbon)) Found &= ~BackyardClue.Returned;
        }
        public bool Has(BackyardClue clue) => (Found & clue) == clue;
        public bool Available(BackyardClue clue, bool unlocked)
        {
            if (!unlocked || Has(clue)) return false;
            if (clue == BackyardClue.ScentTrail || clue == BackyardClue.PawPassage || clue == BackyardClue.Lookout) return true;
            if (clue == BackyardClue.Ribbon) return (Found & Routes) == Routes;
            if (clue == BackyardClue.Returned) return Has(BackyardClue.Ribbon);
            return false;
        }
        public bool TryDiscover(BackyardClue clue, bool unlocked)
        {
            if (!Available(clue, unlocked)) return false;
            Found |= clue; return true;
        }
    }
}
