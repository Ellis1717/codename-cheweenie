using UnityEngine;
namespace CodenameCheweenie
{
    public sealed class HeadquartersBuilder : MonoBehaviour
    {
        public HouseLayout layout;
        public void Build()
        {
            if (!layout) layout = Resources.Load<HouseLayout>("HouseLayout");
            if (!layout) layout = ScriptableObject.CreateInstance<HouseLayout>();
            var wood = new Color(.61f, .48f, .34f);
            var wall = new Color(.78f, .79f, .76f);
            foreach (var room in layout.rooms)
            {
                float y = .04f + room.story * layout.storyHeight;
                var root = new GameObject(room.label + " | floor plan unverified"); root.transform.SetParent(transform, false);
                Vector3 center = new Vector3(room.center.x, y, room.center.y);
                Box(root.transform, "Room floor", center, new Vector3(Mathf.Max(1, room.size.x), .2f, Mathf.Max(1, room.size.y)), room.color);
                if (room.story == 1)
                {
                    // Side railings leave the south landing open for the ramp.
                    Box(root.transform, "Loft side railing", center + new Vector3(room.size.x * .5f, .7f, 0), new Vector3(.12f, 1.2f, room.size.y), wall);
                }
                if (room.northPartition)
                    Box(root.transform, "North partition", center + new Vector3(0, 1.3f, room.size.y * .5f), new Vector3(room.size.x, 2.6f, .15f), wall);
                if (room.photoReference)
                {
                    var photo = Box(root.transform, "User photo reference", center + new Vector3(0, 1.5f, room.size.y * .5f - .1f), new Vector3(1.6f, 1, .02f), Color.white);
                    photo.GetComponent<Renderer>().material.mainTexture = room.photoReference;
                    photo.GetComponent<Collider>().enabled = false;
                }
            }
            float north = 3 + layout.depth * .5f, south = 3 - layout.depth * .5f;
            Box(transform, "North wall", new Vector3(0, 1.7f, north), new Vector3(layout.width, 3.4f, .25f), wall);
            for (int side = -1; side <= 1; side += 2)
                Box(transform, "Side wall", new Vector3(side * layout.width * .5f, 1.7f, 3), new Vector3(.25f, 3.4f, layout.depth), wall);
            // Wide opening includes the ramp approach, rather than blocking it with an entry wall.
            Box(transform, "Entry east frame", new Vector3(layout.width * .5f - 1.5f, 1.7f, south), new Vector3(3, 3.4f, .25f), wall);
            Vector3 rise = layout.rampTop - layout.rampBottom;
            var ramp = Box(transform, "Accessible loft ramp", (layout.rampTop + layout.rampBottom) * .5f - Vector3.up * .13f,
                new Vector3(layout.rampWidth, .25f, rise.magnitude + .4f), wood);
            ramp.transform.rotation = Quaternion.LookRotation(rise.normalized, Vector3.up);
            // Align local Z along the incline. Low rails keep Marty from sliding sideways.
            for (int side = -1; side <= 1; side += 2)
            {
                var rail = Box(transform, "Ramp railing", ramp.transform.position + new Vector3(side * layout.rampWidth * .5f, .5f, 0),
                    new Vector3(.12f, .8f, rise.magnitude), wall);
                rail.transform.rotation = ramp.transform.rotation;
            }
        }
        private static GameObject Box(Transform parent, string label, Vector3 position, Vector3 scale, Color color)
        {
            var go = GameObject.CreatePrimitive(PrimitiveType.Cube); go.name = label; go.transform.SetParent(parent, false);
            go.transform.localPosition = position; go.transform.localScale = scale;
            go.GetComponent<Renderer>().material.color = color; return go;
        }
    }
}
