using UnrealBuildTool;
public class CheweenieUE5LabTarget : TargetRules
{
    public CheweenieUE5LabTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.Add("CheweenieUE5Lab");
    }
}
