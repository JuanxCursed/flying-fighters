// Copyright 2010-2019 Married Games. All Rights Reserved.
using UnrealBuildTool;
using System.Collections.Generic;

public class FlyingFightersTarget : TargetRules
{
	public FlyingFightersTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "FlyingFighters" } );
	}
}
