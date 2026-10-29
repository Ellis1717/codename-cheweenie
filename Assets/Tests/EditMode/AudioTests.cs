using NUnit.Framework;
using UnityEngine;

namespace CodenameCheweenie.Tests
{
    public sealed class AudioTests
    {
        [Test] public void OlderSavesGainMusicWithoutUndoingMute()
        {
            Assert.AreEqual(.35f, AudioPreferences.RestoreMusic(0, 0, .6f));
            Assert.AreEqual(0, AudioPreferences.RestoreMusic(0, 0, 0));
            Assert.AreEqual(0, AudioPreferences.RestoreMusic(1, 0, .6f));
            Assert.AreEqual(.35f, AudioPreferences.RestoreMusic(1, float.NaN, .6f));
            Assert.AreEqual(1, AudioPreferences.RestoreMusic(1, 10, .6f));
        }
        [Test] public void VolumeAdjustmentsReachMuteAndMaximum()
        {
            Assert.AreEqual(.7f, AudioPreferences.Step(.6f, 1, false));
            Assert.AreEqual(.5f, AudioPreferences.Step(.6f, -1, false));
            Assert.AreEqual(0, AudioPreferences.Step(0, -1, false));
            Assert.AreEqual(1, AudioPreferences.Step(1, 1, false));
            Assert.AreEqual(0, AudioPreferences.Step(1, 1, true));
            Assert.AreEqual(.45f, AudioPreferences.Step(.35f, 1, false));
            Assert.AreEqual(1, AudioPreferences.Step(.95f, 1, true));
        }
        [Test] public void BundledMusicIsStereoAndHasMatchingLoopDurations()
        {
            var home = Resources.Load<AudioClip>("Audio/Home");
            var backyard = Resources.Load<AudioClip>("Audio/Backyard");
            Assert.IsNotNull(home); Assert.IsNotNull(backyard);
            Assert.AreEqual(2, home.channels); Assert.AreEqual(2, backyard.channels);
            Assert.AreEqual(home.samples, backyard.samples);
            Assert.AreEqual(40, home.length, .01f);
            Assert.AreEqual(OriginalAudio.SampleRate, home.frequency);
            Assert.AreEqual(AudioClipLoadType.DecompressOnLoad, home.loadType);
        }
        [Test] public void AllSoundCuesHaveBundledAudio()
        {
            foreach (SoundCue cue in System.Enum.GetValues(typeof(SoundCue)))
            {
                var clip = Resources.Load<AudioClip>("Audio/" + cue);
                Assert.IsNotNull(clip, cue.ToString()); Assert.Greater(clip.samples, 0);
            }
        }
    }
}
