using System;

namespace CodenameCheweenie
{
    public static class AudioPreferences
    {
        public const float DefaultMusic = .35f;
        public static float Unit(float value, float fallback)
        { return float.IsNaN(value) || float.IsInfinity(value) ? fallback : Math.Max(0, Math.Min(1, value)); }
        public static float RestoreMusic(int audioVersion, float saved, float oldEffects)
        { return audioVersion < 1 ? oldEffects <= 0 ? 0 : DefaultMusic : Unit(saved, DefaultMusic); }
        public static float Step(float current, int direction, bool wrap)
        {
            float value = Unit(current, 0);
            if (wrap && value >= 1 && direction > 0) return 0;
            return Unit((float)Math.Round(value + .1 * Math.Sign(direction), 2), 0);
        }
    }
}
