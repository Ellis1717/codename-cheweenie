using UnityEngine;

namespace CodenameCheweenie
{
    public enum CameraPreset { Cozy, Explore, Low }
    public sealed class CheweenieCamera : MonoBehaviour
    {
        public Transform target;
        public Vector3 offset=new Vector3(0,5.5f,-8);
        public float sensitivity=1;
        public float Yaw {get;private set;}
        public float Pitch {get;private set;}=27;
        public CameraPreset Preset {get{GameSave.Load();return(CameraPreset)GameSave.Data.cameraMode;}}
        public string PresetName=>Preset==CameraPreset.Cozy?"Cozy":Preset==CameraPreset.Explore?"Explore":"Low";
        private bool positioned;
        private MartyInput controls;
        private QuestDirector quest;
        private NeighbourhoodDirector map;
        private readonly RaycastHit[] hits=new RaycastHit[16];
        private void Start()
        {
            if(target)controls=target.GetComponent<MartyInput>();quest=Object.FindFirstObjectByType<QuestDirector>();map=Object.FindFirstObjectByType<NeighbourhoodDirector>();
            var camera=GetComponent<Camera>();if(camera)camera.nearClipPlane=.06f;
            Pitch=Preset==CameraPreset.Low?8:27;
        }
        public void SnapToTarget(){positioned=false;Yaw=target?target.eulerAngles.y:0;if(Preset==CameraPreset.Cozy)Yaw=0;}
        public Vector3 ToWorldMovement(Vector3 input)=>Preset==CameraPreset.Cozy?input:Quaternion.Euler(0,Yaw,0)*input;
        public void CyclePreset()
        {
            GameSave.Load();GameSave.Data.cameraMode=(GameSave.Data.cameraMode+1)%3;Pitch=Preset==CameraPreset.Low?8:27;
            if(Preset==CameraPreset.Cozy)Yaw=0;
            GameSave.Capture();quest?.Announce("Camera: "+PresetName+". Right stick: look. RB/R1: recenter.");
        }
        public void Recenter(){Yaw=Preset==CameraPreset.Cozy?0:target.eulerAngles.y;Pitch=Preset==CameraPreset.Low?8:27;}
        public void SetOrbit(float yaw,float pitch){Yaw=Mathf.Repeat(yaw,360);Pitch=Mathf.Clamp(pitch,2,70);}
        private void Update()
        {
            if(!target||!controls||!controls.AllowsGameplayUtilities||Time.timeScale<=0||(map&&map.MapExpanded))return;
            if(controls.KeyboardUtilityAvailable(KeyCode.V)&&GameControls.GetKeyDown(KeyCode.V)||controls.PadUtilityAvailable(9,11)&&GameControls.CameraCyclePressed)CyclePreset();
            if(controls.KeyboardUtilityAvailable(KeyCode.Home)&&GameControls.GetKeyDown(KeyCode.Home)||controls.PadUtilityAvailable(5,5)&&GameControls.CameraRecenterPressed)Recenter();
            Vector2 look=GameControls.LookDelta(Time.unscaledDeltaTime)*sensitivity;
            if(look.sqrMagnitude<.0001f)return;
            if(Preset==CameraPreset.Cozy){GameSave.Data.cameraMode=(int)CameraPreset.Explore;Pitch=27;GameSave.Capture();}
            Yaw=Mathf.Repeat(Yaw+look.x,360);Pitch=Mathf.Clamp(Pitch-look.y,Preset==CameraPreset.Low?2:12,Preset==CameraPreset.Low?30:70);
        }
        private void LateUpdate()
        {
            if(!target)return;
            GameSave.Load();Vector3 focus=target.position+Vector3.up*.3f;
            Vector3 displacement=Preset==CameraPreset.Cozy?offset:Quaternion.Euler(Pitch,Yaw,0)*Vector3.back*(Preset==CameraPreset.Low?3.1f:5.5f);
            Vector3 destination=focus+displacement*GameSave.Data.cameraScale;
            destination=ClearView(focus,destination);
            Vector3 blended=positioned?Vector3.Lerp(transform.position,destination,1-Mathf.Exp(-8*Time.unscaledDeltaTime)):destination;
            transform.position=ClearView(focus,blended);positioned=true;transform.LookAt(focus);
        }
        private Vector3 ClearView(Vector3 focus,Vector3 destination)
        {
            Vector3 delta=destination-focus;if(delta.sqrMagnitude<.0001f)return destination;
            int count=Physics.SphereCastNonAlloc(focus,.18f,delta.normalized,hits,delta.magnitude,~(1<<2),QueryTriggerInteraction.Ignore);float distance=delta.magnitude;
            for(int i=0;i<count;i++){if(hits[i].transform==target||hits[i].transform.IsChildOf(target))continue;distance=Mathf.Min(distance,Mathf.Max(.05f,hits[i].distance-.045f));}
            return focus+delta.normalized*distance;
        }
    }
}
