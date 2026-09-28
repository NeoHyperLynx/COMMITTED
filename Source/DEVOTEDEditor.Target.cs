using UnrealBuildTool;
using System.Collections.Generic;

public class DEVOTEDEditorTarget : TargetRules
{
    public DEVOTEDEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        ExtraModuleNames.Add("DEVOTED");
    }
}
