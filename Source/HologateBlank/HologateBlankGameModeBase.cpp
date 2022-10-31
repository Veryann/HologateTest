// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "HologateBlankGameModeBase.h"
#include "HologateCharacter.h"
#include "UObject/ConstructorHelpers.h"


AHologateBlankGameModeBase::AHologateBlankGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/CharacterBP"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

