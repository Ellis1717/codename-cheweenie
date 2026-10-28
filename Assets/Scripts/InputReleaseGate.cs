namespace CodenameCheweenie
{
    public sealed class InputReleaseGate
    {
        private int suppressedFrame = -1;
        public bool WaitingForRelease { get; private set; }
        public void Suppress(int frame) { suppressedFrame = frame; WaitingForRelease = true; }
        public bool CanRead(int frame, bool blocked, bool actionHeld)
        {
            if (blocked || frame <= suppressedFrame) return false;
            if (WaitingForRelease && actionHeld) return false;
            WaitingForRelease = false;
            return true;
        }
    }
}
