using UnityEngine;

namespace CodenameCheweenie
{
    [CreateAssetMenu(menuName="Cheweenie/Scenery Palette")]
    public sealed class SceneryPalette:ScriptableObject
    {
        public Material wood,grass,stone,paving,brick,plaster,fabric,roof,leaf,glass,metal,warmLight;
        public GameObject sofa,table,island,cushion,fence,tree,flowers,window,bench,playground,rug,gabledRoof,shrub;
    }
}
