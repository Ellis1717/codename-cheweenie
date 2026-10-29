using System;
using CodenameCheweenie;

static class AudioChecks
{
    public static void Run(Action<bool> check)
    {
        check(AudioPreferences.RestoreMusic(0, 0, .6f) == .35f);
        check(AudioPreferences.RestoreMusic(0, 0, 0) == 0);
        check(AudioPreferences.RestoreMusic(1, 0, .6f) == 0);
        check(AudioPreferences.RestoreMusic(1, float.NaN, .6f) == .35f);
        check(AudioPreferences.Unit(float.PositiveInfinity, .6f) == .6f);
        check(AudioPreferences.Unit(-4, .6f) == 0);
        check(AudioPreferences.Unit(4, .6f) == 1);
        check(AudioPreferences.Step(.6f, 1, false) == .7f);
        check(AudioPreferences.Step(.6f, -1, false) == .5f);
        check(AudioPreferences.Step(0, -1, false) == 0);
        check(AudioPreferences.Step(1, 1, false) == 1);
        check(AudioPreferences.Step(1, 1, true) == 0);
        check(AudioPreferences.Step(.35f, 1, false) == .45f);
        check(AudioPreferences.Step(.95f, 1, true) == 1);
        foreach (bool garden in new[] { false, true })
        {
            float[] pcm = OriginalAudio.Music(garden);
            check(pcm.Length == 40 * OriginalAudio.SampleRate * 2);
            double power = 0; float peak = 0; bool stereo = false, finite = true;
            foreach (float value in pcm) { finite &= float.IsFinite(value); power += value * value; peak = Math.Max(peak, Math.Abs(value)); }
            for (int i = 0; i < pcm.Length; i += 2) stereo |= Math.Abs(pcm[i] - pcm[i + 1]) > .001f;
            check(finite && peak <= .63f && peak > .5f);
            check(Math.Sqrt(power / pcm.Length) > .03);
            check(stereo);
            check(Math.Abs(pcm[0] - pcm[pcm.Length - 2]) < .04f && Math.Abs(pcm[1] - pcm[pcm.Length - 1]) < .04f);
        }
        foreach (SoundCue cue in Enum.GetValues<SoundCue>())
        {
            float[] pcm = OriginalAudio.Effect(cue); bool valid = true; float peak = 0;
            foreach (float value in pcm) { valid &= float.IsFinite(value) && Math.Abs(value) < 1; peak = Math.Max(peak, Math.Abs(value)); }
            check(valid && peak > .01f && pcm.Length > 100);
        }
    }
}
