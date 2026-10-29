using System.Collections;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.TestTools;

namespace CodenameCheweenie.Tests
{
    public sealed class AudioPlaybackTests
    {
        [UnityTest] public IEnumerator MusicRunsOnTitleWithSeparateVolumesAndBackyardCrossfade()
        {
            GameSave.PersistenceEnabled = false; CheweenieBootstrap.Build();
            yield return null;
            var bus = Object.FindFirstObjectByType<GameAudio>();
            var frontEnd = Object.FindFirstObjectByType<FrontEndMenu>();
            var dog = Object.FindFirstObjectByType<MartyController>();
            AudioSource home = null, garden = null, effects = null;
            foreach (var source in bus.GetComponents<AudioSource>())
            {
                if (!source.loop) effects = source;
                else if (source.clip.name == "Home") home = source;
                else if (source.clip.name == "Backyard") garden = source;
            }
            Assert.IsNotNull(home); Assert.IsNotNull(garden); Assert.IsNotNull(effects);
            GameSave.Data.musicVolume = .4f; GameSave.Data.volume = .6f;
            yield return new WaitForSecondsRealtime(.4f);
            Assert.AreEqual(0, Time.timeScale);
            Assert.IsTrue(home.isPlaying); Assert.IsTrue(garden.isPlaying);
            int sample = home.timeSamples;
            yield return new WaitForSecondsRealtime(.1f);
            Assert.AreNotEqual(sample, home.timeSamples, "Title pause must not freeze the soundtrack");
            bus.Play(SoundCue.Jump);
            Assert.IsFalse(effects.isPlaying, "Gameplay sounds must stay silent on the title");
            bus.Play(SoundCue.Menu);
            yield return new WaitForSecondsRealtime(.02f);
            Assert.IsTrue(effects.isPlaying, "Menu feedback remains audible on the title");
            GameSave.Data.musicVolume = 0;
            yield return new WaitForSecondsRealtime(.06f);
            Assert.AreEqual(0, home.volume); Assert.AreEqual(0, garden.volume);
            Assert.AreEqual(.6f, effects.volume, .001f);
            GameSave.Data.volume = 0; GameSave.Data.musicVolume = .4f;
            yield return new WaitForSecondsRealtime(.06f);
            Assert.AreEqual(0, effects.volume); Assert.Greater(home.volume, 0);
            frontEnd.StartAdventure(false);
            dog.transform.position = new Vector3(0, .7f, -12); Physics.SyncTransforms();
            yield return new WaitForSecondsRealtime(2.1f);
            Assert.Greater(garden.volume, home.volume, "Backyard music should fade in outdoors");
            frontEnd.ShowTitle();
            yield return new WaitForSecondsRealtime(2.1f);
            Assert.Greater(home.volume, garden.volume, "Returning to title restores the home theme");
            LogAssert.NoUnexpectedReceived();
        }
        [UnityTearDown] public IEnumerator Cleanup()
        {
            var bus = Object.FindFirstObjectByType<GameAudio>(); if (bus) Object.Destroy(bus.gameObject);
            yield return null; GameSave.Reload(); GameSave.PersistenceEnabled = true; Time.timeScale = 1;
        }
    }
}
