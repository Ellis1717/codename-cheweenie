using NUnit.Framework;
using UnityEngine;

namespace CodenameCheweenie.Tests
{
    public sealed class RoomLayoutTests
    {
        [Test] public void RuntimeMaterialsReferenceShadersExplicitly()
        {
            var world = Resources.Load<Material>("Branding/World");
            var particles = Resources.Load<Material>("Branding/Sparkles");
            Assert.IsNotNull(world); Assert.IsNotNull(particles);
            Assert.IsNotNull(world.shader); Assert.IsNotNull(particles.shader);
        }
        [Test] public void DefaultRoomsMeetWithoutGaps()
        {
            var layout = ScriptableObject.CreateInstance<HouseLayout>();
            try
            {
                Assert.AreEqual(layout.rooms[0].center.x + layout.rooms[0].size.x * .5f,
                    layout.rooms[1].center.x - layout.rooms[1].size.x * .5f, .001f);
                Assert.AreEqual(layout.rooms[2].center.x + layout.rooms[2].size.x * .5f,
                    layout.rooms[3].center.x - layout.rooms[3].size.x * .5f, .001f);
            }
            finally { Object.DestroyImmediate(layout); }
        }
    }
}
