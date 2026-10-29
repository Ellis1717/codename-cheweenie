using System;
using System.Collections.Generic;
using UnityEngine;
namespace CodenameCheweenie
{
    [Serializable] public sealed class RoomDefinition
    {
        public string label = "Reference pending";
        public Vector2 center;
        public Vector2 size = new Vector2(4, 4);
        [Range(0, 1)] public int story;
        public Color color = new Color(.61f, .48f, .34f);
        public bool northPartition;
        public Texture2D photoReference;
    }
    [CreateAssetMenu(menuName = "Cheweenie/House Layout")]
    public sealed class HouseLayout : ScriptableObject
    {
        [Min(10)] public float width = 17;
        [Min(12)] public float depth = 13;
        [Min(3)] public float storyHeight = 3.8f;
        public List<RoomDefinition> rooms = new List<RoomDefinition>
        {
            new RoomDefinition { label = "Schematic lounge", center = new Vector2(-4.25f, 3), size = new Vector2(8.5f, 12.6f) },
            new RoomDefinition { label = "Schematic kitchen", center = new Vector2(4.25f, 3), size = new Vector2(8.5f, 12.6f) },
            new RoomDefinition { label = "Schematic upstairs nook", center = new Vector2(-6.3f, 5.2f), size = new Vector2(3.6f, 7.6f), story = 1, northPartition = true },
            new RoomDefinition { label = "Schematic upstairs landing", center = new Vector2(-2.7f, 5.2f), size = new Vector2(3.6f, 7.6f), story = 1 }
        };
        public Vector3 rampBottom = new Vector3(-6.3f, -.1f, -6.5f);
        public Vector3 rampTop = new Vector3(-6.3f, 3.94f, 1.8f);
        [Min(1.5f)] public float rampWidth = 2;
    }
}
