// OnlineMultiplayerEditor.Target.cs

using UnrealBuildTool;
using System.Collections.Generic;

public class OnlineMultiplayerEditorTarget : TargetRules
{
	public OnlineMultiplayerEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
		ExtraModuleNames.Add("OnlineMultiplayer");
	}
}