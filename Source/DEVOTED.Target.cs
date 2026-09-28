using UnrealBuildTool;
using System.Collections.Generic;

public class DEVOTEDTarget : TargetRules
{
    public DEVOTEDTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        ExtraModuleNames.Add("DEVOTED");
    }
}
