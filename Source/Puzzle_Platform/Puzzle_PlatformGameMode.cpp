// Copyright Epic Games, Inc. All Rights Reserved.

#include "Puzzle_PlatformGameMode.h"
#include "Puzzle_PlatformCharacter.h"
#include "UObject/ConstructorHelpers.h"

APuzzle_PlatformGameMode::APuzzle_PlatformGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
