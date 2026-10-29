using NUnit.Framework;
using UnityEngine;

namespace CodenameCheweenie.Tests
{
    public sealed class CatLookTests
    {
        [Test] public void LakituHasAnAsymmetricMaskAndWhiteBlaze()
        {
            Assert.Less(CatVisual.FaceCoat(CatLook.Lakitu, new Vector3(-.5f, .1f, .85f)).grayscale, .1f);
            Assert.Greater(CatVisual.FaceCoat(CatLook.Lakitu, new Vector3(.5f, .1f, .85f)).grayscale, .8f);
            Assert.Greater(CatVisual.FaceCoat(CatLook.Lakitu, new Vector3(0, .55f, .85f)).grayscale, .8f);
        }
        [Test] public void RioKeepsABlackFaceAndWhiteChin()
        {
            Assert.Less(CatVisual.FaceCoat(CatLook.Rio, Vector3.forward).grayscale, .1f);
            Assert.Less(CatVisual.FaceCoat(CatLook.Rio, new Vector3(.5f, .1f, .85f)).grayscale, .1f);
            Assert.Greater(CatVisual.FaceCoat(CatLook.Rio, new Vector3(0, -.7f, .7f)).grayscale, .8f);
        }
        [Test] public void TheCatsDoNotCollapseIntoTheSameCoatPattern()
        { Assert.AreNotEqual(CatVisual.FaceCoat(CatLook.Lakitu, Vector3.forward), CatVisual.FaceCoat(CatLook.Rio, Vector3.forward)); }
        [Test] public void LakituKeepsAWhiteFrontForeheadAwayFromTheLeftMask()
        {Assert.Greater(CatVisual.FaceCoat(CatLook.Lakitu,new Vector3(.3f,.55f,.78f)).grayscale,.8f);}
    }
}
