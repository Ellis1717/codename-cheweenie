using System;

namespace CodenameCheweenie
{
    [Flags] public enum NeighbourhoodStop { None = 0, Mailbox = 1, Garden = 2, Park = 4, Home = 8, School = 16 }

    public sealed class NeighbourhoodProgress
    {
        public int State { get; private set; }
        private readonly bool includesSchool;
        public int RequiredVisits => includesSchool ? 4 : 3;
        public int Visits => (Has(NeighbourhoodStop.Mailbox) ? 1 : 0) + (Has(NeighbourhoodStop.Garden) ? 1 : 0) + (Has(NeighbourhoodStop.Park) ? 1 : 0) + (includesSchool && Has(NeighbourhoodStop.School) ? 1 : 0);
        public bool Complete => Has(NeighbourhoodStop.Home);
        public NeighbourhoodProgress(int saved = 0, bool school = false)
        {
            includesSchool = school;
            State = saved < 0 ? 0 : saved & 31;
            int required = school ? 23 : 7;
            if ((State & required) != required) State &= ~8;
        }
        public bool Has(NeighbourhoodStop stop) => stop != NeighbourhoodStop.None && (State & (int)stop) == (int)stop;
        public bool Visit(NeighbourhoodStop stop)
        {
            if (stop != NeighbourhoodStop.Mailbox && stop != NeighbourhoodStop.Garden && stop != NeighbourhoodStop.Park && stop != NeighbourhoodStop.Home && !(includesSchool && stop == NeighbourhoodStop.School)) return false;
            if (Has(stop) || (stop == NeighbourhoodStop.Home && Visits != RequiredVisits)) return false;
            State |= (int)stop; return true;
        }
    }
}
