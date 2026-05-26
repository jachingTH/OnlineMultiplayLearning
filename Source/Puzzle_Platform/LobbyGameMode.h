// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Puzzle_PlatformGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLE_PLATFORM_API ALobbyGameMode : public APuzzle_PlatformGameMode
{
	GENERATED_BODY()
	
public:
	void PostLogin(APlayerController* NewPlayer) override;

	void Logout(AController* Exiting) override;

private:

	void StartGame();

	uint32 NumberOfPlayers = 0;

	FTimerHandle GameStartTimer;
};
