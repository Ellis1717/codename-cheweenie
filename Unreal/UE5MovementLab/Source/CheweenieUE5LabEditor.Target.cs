using UnrealBuildTool;
public class CheweenieUE5LabEditorTarget : TargetRules
{
    public CheweenieUE5LabEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.Add("CheweenieUE5Lab");
    }
}
