using UnityEngine;

namespace CodenameCheweenie
{
    // Distance-driven procedural contacts for the supplied articulated static meshes.
    // Root locomotion remains on the original Rigidbody; this changes only visual joints.
    public sealed class GroundedGait:MonoBehaviour
    {
        public float Cycle {get;private set;}
        public float Travelled {get;private set;}
        public bool Grounded {get;private set;}
        public int PlantedFeet {get;private set;}
        public int Steps {get;private set;}
        public LocomotionTuning tuning;
        private Transform owner;
        private Rigidbody body;
        private Transform[] hips,ankles;
        private Vector3[] hipPositions,anklePositions,hipScales,ankleScales,soles,anchors,swingStarts;
        private Quaternion[] hipRotations,ankleRotations;
        private bool[] planted;
        private int[] contactIds;
        private Vector3 previous,restPosition;
        private Quaternion restRotation;
        private float smoothedSpeed,cozyBlend,landing;
        private float soleOffset;
        private float previousYaw;
        private bool configured,ownedTuning;
        private readonly RaycastHit[] hits=new RaycastHit[12];
        public void Configure(Transform[] legJoints,Transform[] footJoints,string profile)
        {
            if(configured)return;
            hips=legJoints;ankles=footJoints;body=GetComponentInParent<Rigidbody>();owner=body?body.transform:transform.parent;
            tuning=Resources.Load<LocomotionTuning>("Motion/"+profile);
            if(!tuning){tuning=ScriptableObject.CreateInstance<LocomotionTuning>();ownedTuning=true;if(profile!="Marty"){tuning.walkStride=.36f;tuning.trotStride=.62f;tuning.runStride=.85f;tuning.trotSpeed=1.3f;tuning.runSpeed=3;tuning.pawLift=.065f;tuning.maxReach=.25f;tuning.bodyBob=.008f;tuning.bodyRoll=.8f;tuning.maxStretch=1.35f;}}
            int count=hips.Length;hipPositions=new Vector3[count];anklePositions=new Vector3[count];hipScales=new Vector3[count];ankleScales=new Vector3[count];hipRotations=new Quaternion[count];ankleRotations=new Quaternion[count];soles=new Vector3[count];anchors=new Vector3[count];swingStarts=new Vector3[count];planted=new bool[count];contactIds=new int[count];
            for(int i=0;i<count;i++)
            {
                hipPositions[i]=hips[i].localPosition;anklePositions[i]=ankles[i].localPosition;hipScales[i]=hips[i].localScale;ankleScales[i]=ankles[i].localScale;hipRotations[i]=hips[i].localRotation;ankleRotations[i]=Quaternion.Inverse(transform.rotation)*ankles[i].rotation;
                var renderers=ankles[i].GetComponentsInChildren<Renderer>();Bounds bounds=new Bounds(ankles[i].position,Vector3.zero);foreach(var renderer in renderers)bounds.Encapsulate(renderer.bounds);
                soles[i]=ankles[i].InverseTransformPoint(new Vector3(bounds.center.x,bounds.min.y,bounds.center.z));
                soleOffset+=owner.InverseTransformPoint(ankles[i].TransformPoint(soles[i])).y/count;
            }
            restPosition=transform.localPosition;restRotation=transform.localRotation;previous=owner.position;configured=true;ResetContacts();
        }
        public void ResetContacts()
        {
            if(!configured)return;
            previous=owner.position;previousYaw=owner.eulerAngles.y;Cycle=0;smoothedSpeed=0;
            for(int i=0;i<planted.Length;i++){planted[i]=false;contactIds[i]++;}
        }
        public Vector3 SolePosition(int leg)=>ankles[leg].TransformPoint(soles[leg]);
        public bool IsPlanted(int leg)=>planted[leg];
        public int ContactId(int leg)=>contactIds[leg];
        public void Animate(bool cozy,bool between)
        {
            if(!configured||Time.deltaTime<=0)return;
            float dt=Time.deltaTime;Vector3 delta=owner.position-previous;previous=owner.position;delta.y=0;
            float turn=Mathf.DeltaAngle(previousYaw,owner.eulerAngles.y)*Mathf.Deg2Rad;previousYaw=owner.eulerAngles.y;
            if(delta.magnitude>2){ResetContacts();delta=Vector3.zero;}
            float travel=delta.magnitude+Mathf.Abs(turn)*.12f;
            float speed=travel/Mathf.Max(dt,.001f);smoothedSpeed=Mathf.Lerp(smoothedSpeed,speed,1-Mathf.Exp(-12*dt));
            float stride=smoothedSpeed>=tuning.runSpeed?tuning.runStride:smoothedSpeed>=tuning.trotSpeed?tuning.trotStride:tuning.walkStride;
            if(!cozy&&!between){Travelled+=delta.magnitude;Cycle=Mathf.Repeat(Cycle+travel/Mathf.Max(.1f,stride),1);}
            cozyBlend=Mathf.MoveTowards(cozyBlend,cozy?1:0,dt*5);
            bool wasGrounded=Grounded;Grounded=Ground(owner.position+Vector3.up*(soleOffset+.2f),out var center)&&Mathf.Abs(center.point.y-owner.TransformPoint(Vector3.up*soleOffset).y)<.24f&&(!body||Vector3.Dot(body.linearVelocity,center.normal)<1);
            if(Grounded&&!wasGrounded)landing=.035f;landing=Mathf.MoveTowards(landing,0,dt*.24f);
            float amount=Mathf.Clamp01(smoothedSpeed/1.2f)*(1-cozyBlend);
            float bob=Grounded?Mathf.Sin(Cycle*Mathf.PI*4)*tuning.bodyBob*amount-landing:0;
            if(smoothedSpeed<.12f)bob+=Mathf.Sin(Time.time*1.8f)*.004f;
            transform.localPosition=Vector3.Lerp(transform.localPosition,restPosition+Vector3.up*bob,1-Mathf.Exp(-14*dt));
            transform.localRotation=Quaternion.Slerp(transform.localRotation,restRotation*Quaternion.Euler(0,0,Mathf.Sin(Cycle*Mathf.PI*2)*tuning.bodyRoll*amount),1-Mathf.Exp(-12*dt));
            PlantedFeet=0;Vector3 forward=delta.sqrMagnitude>.000001f?delta.normalized:owner.forward;
            for(int i=0;i<hips.Length;i++)
            {
                hips[i].localPosition=hipPositions[i];hips[i].localRotation=hipRotations[i];hips[i].localScale=hipScales[i];ankles[i].localPosition=anklePositions[i];ankles[i].localScale=ankleScales[i];ankles[i].rotation=transform.rotation*ankleRotations[i];
                if(cozyBlend>.02f||between||!Grounded)
                {
                    planted[i]=false;
                    float fold=cozyBlend>0?65*cozyBlend:!Grounded?(i<2?18:-12):0;
                    hips[i].localRotation=hipRotations[i]*Quaternion.Euler(fold,0,0);ankles[i].localRotation=Quaternion.Euler(cozyBlend>0?-70*cozyBlend:0,0,0);continue;
                }
                Vector3 restSole=SolePosition(i);float phase=LocomotionTuning.LegPhase(Cycle,i,smoothedSpeed,tuning);float stance=LocomotionTuning.StanceFraction(smoothedSpeed,tuning);
                Vector3 stepDirection=delta.sqrMagnitude<.00001f&&Mathf.Abs(turn)>.0001f?Vector3.Cross(Vector3.up,restSole-owner.position).normalized*Mathf.Sign(turn):forward;
                bool contact=smoothedSpeed<.10f||phase<stance;Vector3 normal=Vector3.up;
                if(planted[i]&&Vector3.ProjectOnPlane(anchors[i]-restSole,Vector3.up).magnitude>tuning.maxReach){planted[i]=false;contactIds[i]++;}
                Vector3 foot;
                if(contact)
                {
                    if(!planted[i])
                    {
                        Vector3 lead=smoothedSpeed<.1f?Vector3.zero:stepDirection*Mathf.Min(tuning.maxReach*.60f,stride*.25f);
                        if(Ground(restSole+lead,out var hit)){anchors[i]=hit.point+hit.normal*.005f;normal=hit.normal;}else anchors[i]=restSole;
                        planted[i]=true;contactIds[i]++;Steps++;
                    }
                    foot=anchors[i];PlantedFeet++;
                }
                else
                {
                    if(planted[i]){swingStarts[i]=SolePosition(i);planted[i]=false;contactIds[i]++;}
                    float swing=Mathf.Clamp01((phase-stance)/(1-stance));Vector3 finish=restSole+stepDirection*Mathf.Min(tuning.maxReach*.65f,stride*.26f);
                    if(Ground(finish,out var hit)){finish=hit.point+hit.normal*.005f;normal=hit.normal;}
                    if(swingStarts[i]==Vector3.zero)swingStarts[i]=restSole;
                    foot=Vector3.Lerp(swingStarts[i],finish,Mathf.SmoothStep(0,1,swing))+Vector3.up*(Mathf.Sin(swing*Mathf.PI)*tuning.pawLift);
                }
                Quaternion rotation=Quaternion.FromToRotation(Vector3.up,normal)*transform.rotation*ankleRotations[i];
                Vector3 offset=ankles[i].TransformVector(soles[i]);Vector3 goal=foot-offset;Vector3 wanted=hips[i].parent.InverseTransformVector(goal-hips[i].position);
                Vector3 rest=hipRotations[i]*anklePositions[i];float stretch=Mathf.Clamp(wanted.magnitude/Mathf.Max(.08f,rest.magnitude),.85f,tuning.maxStretch);
                hips[i].localRotation=Quaternion.FromToRotation(rest,wanted)*hipRotations[i];hips[i].localScale=Vector3.Scale(hipScales[i],new Vector3(1,stretch,1));
                ankles[i].localScale=Vector3.Scale(ankleScales[i],new Vector3(1,1/stretch,1));ankles[i].rotation=rotation;
                ankles[i].position=foot-ankles[i].TransformVector(soles[i]);
            }
        }
        private bool Ground(Vector3 point,out RaycastHit ground)
        {
            int count=Physics.RaycastNonAlloc(point+Vector3.up*.45f,Vector3.down,hits,1.1f,~(1<<2),QueryTriggerInteraction.Ignore);float best=float.MaxValue;ground=default;
            for(int i=0;i<count;i++){var hit=hits[i];if(hit.transform.IsChildOf(owner)||hit.normal.y<.65f||hit.distance>=best)continue;best=hit.distance;ground=hit;}
            return best<float.MaxValue;
        }
        private void OnDestroy(){if(ownedTuning&&tuning)Destroy(tuning);}
    }
}
