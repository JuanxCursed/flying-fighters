// Copyright 2010-2019 Married Games. All Rights Reserved.

#include "FlyingFightersGameModeBase.h"
//#include "FlyingFighters/Public/FlyingFightersPawn.h"
#include "UObject/ConstructorHelpers.h"

AFlyingFightersGameModeBase::AFlyingFightersGameModeBase(){
    // set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
    //DefaultPawnClass = AFlyingFightersPawn::StaticClass();
}