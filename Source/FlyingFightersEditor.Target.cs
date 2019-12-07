// Copyright 2010-2019 Married Games. All Rights Reserved.
using UnrealBuildTool;
using System.Collections.Generic;

public class FlyingFightersEditorTarget : TargetRules
{
	public FlyingFightersEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "FlyingFighters" } );
	}
}
