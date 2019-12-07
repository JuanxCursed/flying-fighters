// Copyright 2010-2019 Married Games. All Rights Reserved.

#include "FlyingFightersGameModeBase.h"
#include "FlyingFighters/Public/FlyingFightersPawn.h"
#include "UObject/ConstructorHelpers.h"

AFlyingFightersGameModeBase::AFlyingFightersGameModeBase(){
    DefaultPawnClass = AFlyingFightersPawn::StaticClass();
}