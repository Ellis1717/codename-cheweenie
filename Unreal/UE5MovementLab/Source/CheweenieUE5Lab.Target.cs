using UnrealBuildTool;
public class CheweenieUE5LabTarget : TargetRules
{
    public CheweenieUE5LabTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        ExtraModuleNames.Add("CheweenieUE5Lab");
    }
}
