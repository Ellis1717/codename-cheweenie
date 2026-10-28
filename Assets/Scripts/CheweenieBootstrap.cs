using UnityEngine;

namespace CodenameCheweenie
{
    public static class CheweenieBootstrap
    {
        [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.AfterSceneLoad)]
        private static void Build()
        {
            if (Object.FindFirstObjectByType<QuestDirector>() != null) return;
            GameObject world = new GameObject("Codename .... Cheweenie | Prototype World");
            var quest = world.AddComponent<QuestDirector>();
            MakeSun();
            Primitive("Lawn", PrimitiveType.Cube, new Vector3(0, -0.35f, 0), new Vector3(44, 0.5f, 44), new Color(.22f, .48f, .24f));
            var headquarters = new GameObject("Modular headquarters | schematic");
            headquarters.AddComponent<HeadquartersBuilder>().Build();
            MakeHouse();
            MakeCharacter("Lakitu (Keetz / Keeterman)", new Vector3(-5, .65f, 4), new Color(.25f, .25f, .25f), "Keetz found! The Keeterman approves.", 0);
            MakeCharacter("Rio (Sis / Little Miss / Miss Priss)", new Vector3(5, .65f, 5), new Color(.85f, .73f, .63f), "Little Miss Priss has inspected you.", 1);
            Target("Backyard sniffing zone", new Vector3(1, .75f, -9), new Color(.95f, .76f, .15f), 2, "Backyard fully sniffed!");
            Target("Cheeween Cozy blanket fort", new Vector3(0, .65f, 6.8f), new Color(.24f, .35f, .68f), 3, "Cheeween Cozy achieved! Mission complete.");
            var dog = new GameObject("Marty | Marty Man | Marty Party | Mr. Marty");
            dog.transform.position = new Vector3(0, .9f, -1);
            var capsule = dog.AddComponent<CapsuleCollider>();
            capsule.height = 1.3f; capsule.radius = .42f; capsule.center = new Vector3(0, -.15f, 0);
            var rb = dog.AddComponent<Rigidbody>();
            rb.mass = 3; rb.constraints = RigidbodyConstraints.FreezeRotation;
            dog.AddComponent<MartyController>().Initialize(quest);
            dog.AddComponent<CanineVisual>();
            var cameraObject = new GameObject("Follow Camera");
            var cam = cameraObject.AddComponent<Camera>(); cam.tag = "MainCamera";
            cameraObject.AddComponent<AudioListener>();
            cameraObject.AddComponent<CheweenieCamera>().target = dog.transform;
            quest.SetPlayer(dog.transform);
        }

        private static void MakeSun()
        {
            var light = new GameObject("Daylight").AddComponent<Light>();
            light.type = LightType.Directional; light.intensity = 1.3f;
            light.transform.rotation = Quaternion.Euler(48, -28, 0);
            RenderSettings.ambientLight = new Color(.75f, .77f, .83f);
        }

        private static void MakeHouse()
        {
            // Broad open-sided floor plan so the first quest is fully navigable.
            // Geometry is deliberately schematic; real house measurements/photos are needed.
            Primitive("Sofa", PrimitiveType.Cube, new Vector3(-3.1f, .7f, 6.4f), new Vector3(2.5f, 1.1f, 1), new Color(.32f, .38f, .45f));
            Primitive("Coffee table", PrimitiveType.Cube, new Vector3(-3, .4f, 4.1f), new Vector3(1.7f, .65f, 1.1f), new Color(.35f, .25f, .17f));
            Primitive("Kitchen island", PrimitiveType.Cube, new Vector3(3.5f, .8f, 1.3f), new Vector3(2.5f, 1.4f, 1), new Color(.88f, .84f, .74f));
            Primitive("Blanket canopy", PrimitiveType.Cube, new Vector3(0, 1.65f, 6.8f), new Vector3(2.5f, .15f, 2.8f), new Color(.18f, .27f, .58f)).GetComponent<Collider>().enabled = false;
            Primitive("Patio", PrimitiveType.Cube, new Vector3(0, -.05f, -6), new Vector3(5, .12f, 4), new Color(.62f, .59f, .52f));
        }

        private static void MakeCharacter(string name, Vector3 position, Color color, string line, int stage)
        {
            var cat = Primitive(name, PrimitiveType.Capsule, position, new Vector3(.75f, 1f, .9f), color);
            var target = cat.AddComponent<InteractableTarget>();
            target.stage = stage; target.dialogue = line;
            PrimitiveChild(cat.transform, "head", PrimitiveType.Sphere, new Vector3(0, .42f, .35f), new Vector3(.82f, .65f, .68f), color, false);
            PrimitiveChild(cat.transform, "left ear", PrimitiveType.Cube, new Vector3(-.28f, .78f, .38f), new Vector3(.2f, .3f, .18f), color, false);
            PrimitiveChild(cat.transform, "right ear", PrimitiveType.Cube, new Vector3(.28f, .78f, .38f), new Vector3(.2f, .3f, .18f), color, false);
        }

        private static void Target(string name, Vector3 position, Color color, int stage, string text)
        {
            var go = Primitive(name, PrimitiveType.Cylinder, position, new Vector3(1.2f, .1f, 1.2f), color);
            go.GetComponent<Collider>().enabled = false;
            var target = go.AddComponent<InteractableTarget>(); target.stage = stage; target.dialogue = text;
        }

        private static GameObject Primitive(string name, PrimitiveType kind, Vector3 position, Vector3 scale, Color color)
        {
            var go = GameObject.CreatePrimitive(kind);
            go.name = name; go.transform.position = position; go.transform.localScale = scale;
            ApplyColor(go, color);
            return go;
        }

        private static void PrimitiveChild(Transform parent, string name, PrimitiveType kind, Vector3 localPosition, Vector3 scale, Color color, bool collider)
        {
            var go = GameObject.CreatePrimitive(kind); go.name = name; go.transform.SetParent(parent, false);
            go.transform.localPosition = localPosition; go.transform.localScale = scale;
            if (!collider) Object.Destroy(go.GetComponent<Collider>());
            ApplyColor(go, color);
        }

        private static void ApplyColor(GameObject go, Color color)
        {
            var shader = UnityEngine.Rendering.GraphicsSettings.currentRenderPipeline == null
                ? Shader.Find("Standard") : Shader.Find("Universal Render Pipeline/Lit");
            if (shader == null) return;
            var material = new Material(shader);
            material.color = color;
            go.GetComponent<Renderer>().material = material;
        }
    }
}
