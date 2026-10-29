using System;

namespace CodenameCheweenie
{
    [Flags] public enum NeighbourhoodStop { None = 0, Mailbox = 1, Garden = 2, Park = 4, Home = 8 }

    public sealed class NeighbourhoodProgress
    {
        public int State { get; private set; }
        public int Visits => (Has(NeighbourhoodStop.Mailbox) ? 1 : 0) + (Has(NeighbourhoodStop.Garden) ? 1 : 0) + (Has(NeighbourhoodStop.Park) ? 1 : 0);
        public bool Complete => Has(NeighbourhoodStop.Home);
        public NeighbourhoodProgress(int saved = 0)
        {
            State = saved < 0 ? 0 : saved & 15;
            if ((State & 7) != 7) State &= ~8;
        }
        public bool Has(NeighbourhoodStop stop) => stop != NeighbourhoodStop.None && (State & (int)stop) == (int)stop;
        public bool Visit(NeighbourhoodStop stop)
        {
            if (stop != NeighbourhoodStop.Mailbox && stop != NeighbourhoodStop.Garden && stop != NeighbourhoodStop.Park && stop != NeighbourhoodStop.Home) return false;
            if (Has(stop) || (stop == NeighbourhoodStop.Home && Visits != 3)) return false;
            State |= (int)stop; return true;
        }
    }
}
