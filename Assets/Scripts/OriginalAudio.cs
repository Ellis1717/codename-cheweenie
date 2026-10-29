using System;

namespace CodenameCheweenie
{
    public enum SoundCue { Menu, Step, Jump, Land, Lakitu, Rio, Cozy, Discovery, Between }

    // Original score/synthesis, with no Unity or third-party dependencies. Rendered to bundled WAVs by Validation/AudioAssets.
    public static class OriginalAudio
    {
        public const int SampleRate = 22050;
        public const int Bars = 16;
        public const double Beat = .625; // 96 BPM, 4/4, forty-second loops.
        private static double Frequency(int midi) => 440 * Math.Pow(2, (midi - 69) / 12.0);
        private static double Sine(double phase) => Math.Sin(phase * Math.PI * 2);
        public static float[] Music(bool backyard)
        {
            int frames = (int)(Bars * 4 * Beat * SampleRate);
            var pcm = new float[frames * 2];
            int[][] chords = { new[] { 48, 60, 64, 67, 71 }, new[] { 45, 57, 60, 64, 67 }, new[] { 41, 53, 57, 60, 64 }, new[] { 43, 55, 59, 62, 69 } };
            int[][] home = { new[] { 72, 0, 76, 79, 0, 76, 74, 0 }, new[] { 69, 0, 72, 76, 0, 79, 76, 0 },
                new[] { 69, 0, 72, 77, 0, 76, 72, 0 }, new[] { 71, 0, 74, 79, 0, 74, 72, 0 } };
            int[][] garden = { new[] { 79, 81, 0, 84, 79, 0, 76, 0 }, new[] { 76, 79, 0, 81, 76, 0, 72, 0 },
                new[] { 77, 79, 0, 81, 77, 0, 72, 0 }, new[] { 74, 79, 0, 81, 79, 0, 74, 0 } };
            for (int bar = 0; bar < Bars; bar++)
            {
                double start = bar * 4 * Beat;
                int[] chord = chords[bar % 4], melody = (backyard ? garden : home)[bar % 4];
                Add(pcm, start, chord[0], 3.0, .10, 0, 0, true);
                for (int n = 1; n < chord.Length; n++) Add(pcm, start + (n - 1) * .04, chord[n], 3.8, .032, (n % 2 == 0 ? .45 : -.45), 1, true);
                for (int slot = 0; slot < 8; slot++)
                {
                    double at = start + slot * Beat * .5;
                    if (melody[slot] != 0 && (backyard || bar % 4 != 3 || slot < 5))
                        Add(pcm, at, melody[slot] + (bar >= 8 && slot == 3 ? -12 : 0), 2.4, backyard ? .085 : .095, (slot % 3 - 1) * .24, 0, true);
                    if (backyard)
                    {
                        Add(pcm, at, chord[1 + slot % 4] + 12, .45, .025, slot % 2 == 0 ? -.6 : .6, 2, true);
                    }
                }
            }
            Normalize(pcm, .62f); return pcm;
        }
        public static float[] Effect(SoundCue cue)
        {
            double duration = cue == SoundCue.Menu ? .12 : cue == SoundCue.Step ? .15 : cue == SoundCue.Land ? .24 : .85;
            var stereo = new float[(int)(duration * SampleRate) * 2];
            if (cue == SoundCue.Menu) Add(stereo, 0, 83, .1, .14, 0, 2, false);
            else if (cue == SoundCue.Discovery || cue == SoundCue.Cozy || cue == SoundCue.Between)
            {
                int root = cue == SoundCue.Cozy ? 72 : cue == SoundCue.Between ? 74 : 76;
                Add(stereo, 0, root, .72, .15, -.25, 0, false);
                Add(stereo, .11, root + 4, .63, .12, .25, 0, false);
                Add(stereo, .22, root + 7, .60, .10, 0, 0, false);
            }
            else
            {
                for (int i = 0; i < stereo.Length / 2; i++)
                {
                    double t = i / (double)SampleRate, value;
                    double envelope = Math.Min(1, t * 300) * Math.Pow(Math.Max(0, 1 - t / duration), 3);
                    if (cue == SoundCue.Step || cue == SoundCue.Land)
                        value = (Sine((cue == SoundCue.Land ? 78 : 130) * t) + .25 * Sine(371 * t)) * Math.Exp(-t * 35) * envelope * .25;
                    else if (cue == SoundCue.Jump)
                        value = Sine(360 * t + 250 * t * t) * Math.Exp(-t * 8) * envelope * .22;
                    else
                    {
                        double baseHz = cue == SoundCue.Lakitu ? 850 : 650;
                        double phase = baseHz * t + (cue == SoundCue.Lakitu ? 430 : -180) * t * t;
                        value = (Sine(phase) + .15 * Sine(phase * 2)) * envelope * .14;
                    }
                    stereo[i * 2] = stereo[i * 2 + 1] = (float)value;
                }
            }
            // Preserve quiet steps/menu sounds rather than normalizing every cue to the same loudness.
            return stereo;
        }
        private static void Add(float[] pcm, double start, int midi, double duration, double gain, double pan, int voice, bool wrap)
        {
            int first = (int)Math.Round(start * SampleRate), count = (int)(duration * SampleRate), frames = pcm.Length / 2;
            double hz = Frequency(midi), left = Math.Sqrt((1 - pan) * .5), right = Math.Sqrt((1 + pan) * .5);
            for (int i = 0; i < count; i++)
            {
                int frame = first + i;
                if (!wrap && frame >= frames) break;
                frame %= frames;
                double t = i / (double)SampleRate;
                double release = Math.Min(1, (duration - t) / .08);
                double envelope = voice == 1 ? Math.Min(1, t / .35) * Math.Min(1, (duration - t) / .9) : Math.Min(1, t / .006) * Math.Exp(-t * (voice == 2 ? 13 : 2.5));
                double tone = Sine(hz * t) + (voice == 1 ? .06 : .18) * Sine(hz * 2 * t) + .04 * Sine(hz * 3 * t);
                double value = tone * gain * envelope * release;
                pcm[frame * 2] += (float)(value * left); pcm[frame * 2 + 1] += (float)(value * right);
            }
        }
        private static void Normalize(float[] pcm, float peak)
        {
            float max = 0; foreach (float sample in pcm) max = Math.Max(max, Math.Abs(sample));
            if (max == 0) return;
            for (int i = 0; i < pcm.Length; i++) pcm[i] *= peak / max;
        }
    }
}
