using System;
using System.IO;
using CodenameCheweenie;

static class AudioAssets
{
    static void Main()
    {
        string folder = Path.GetFullPath("Assets/Resources/Audio"); Directory.CreateDirectory(folder);
        Write(folder, "Home", OriginalAudio.Music(false)); Write(folder, "Backyard", OriginalAudio.Music(true));
        foreach (SoundCue cue in Enum.GetValues<SoundCue>()) Write(folder, cue.ToString(), OriginalAudio.Effect(cue));
    }
    static void Write(string folder, string name, float[] pcm)
    {
        float peak = 0; double power = 0;
        foreach (float sample in pcm)
        {
            if (!float.IsFinite(sample) || Math.Abs(sample) > 1) throw new Exception(name + " has invalid/clipped PCM");
            peak = Math.Max(peak, Math.Abs(sample)); power += sample * sample;
        }
        if (peak < .01f) throw new Exception(name + " is silent");
        string staging = Path.GetFullPath("Validation/obj/AudioRender"); Directory.CreateDirectory(staging);
        string temporary = Path.Combine(staging, name + ".wav");
        using var writer = new BinaryWriter(File.Create(temporary));
        writer.Write(System.Text.Encoding.ASCII.GetBytes("RIFF")); writer.Write(36 + pcm.Length * 2);
        writer.Write(System.Text.Encoding.ASCII.GetBytes("WAVEfmt ")); writer.Write(16); writer.Write((short)1); writer.Write((short)2);
        writer.Write(OriginalAudio.SampleRate); writer.Write(OriginalAudio.SampleRate * 4); writer.Write((short)4); writer.Write((short)16);
        writer.Write(System.Text.Encoding.ASCII.GetBytes("data")); writer.Write(pcm.Length * 2);
        foreach (float sample in pcm) writer.Write((short)Math.Round(sample * short.MaxValue));
        writer.Flush(); writer.Dispose();
        File.Move(temporary, Path.Combine(folder, name + ".wav"), true);
        Console.WriteLine(name + ": " + (pcm.Length / (2.0 * OriginalAudio.SampleRate)).ToString("0.00") + "s stereo; peak " + peak.ToString("0.000") + "; RMS " + Math.Sqrt(power / pcm.Length).ToString("0.000"));
    }
}
