using UnityEngine;
namespace CodenameCheweenie
{
    public sealed class CozyBlanket : MonoBehaviour
    {
        private MartyController dog;
        private GameObject blanket;
        private void Start()
        {
            dog = GetComponent<MartyController>(); blanket = GameObject.CreatePrimitive(PrimitiveType.Cube);
            blanket.name = "Cheeween Cozy blanket"; blanket.transform.SetParent(transform, false);
            blanket.transform.localPosition = new Vector3(0, -.2f, -.05f);
            blanket.transform.localScale = new Vector3(.85f, .17f, 1.25f);
            blanket.GetComponent<Collider>().enabled = false;
            blanket.GetComponent<Renderer>().material.color = new Color(.24f, .35f, .68f);
            blanket.SetActive(false);
        }
        private void LateUpdate() { if (blanket) blanket.SetActive(dog && dog.IsCozy); }
    }
}
