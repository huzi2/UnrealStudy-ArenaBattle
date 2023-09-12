// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameMode.h"
#include "ABCharacter.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABGameStateBase.h"
#include "EngineUtils.h"

AABGameMode::AABGameMode()
	: ScoreToClear(2)
{
	DefaultPawnClass = AABCharacter::StaticClass();
	PlayerControllerClass = AABPlayerController::StaticClass();
	PlayerStateClass = AABPlayerState::StaticClass();
	GameStateClass = AABGameStateBase::StaticClass();
}

void AABGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ABGameState = Cast<AABGameStateBase>(GameState);
}

void AABGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!NewPlayer) return;

	AABPlayerState* ABPlayerState = Cast<AABPlayerState>(NewPlayer->PlayerState);
	ABCHECK(ABPlayerState);
	ABPlayerState->InitPlayerData();
}

void AABGameMode::AddScore(AABPlayerController* ScoredPlayer)
{
	if (!ScoredPlayer) return;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AABPlayerController* ABPlayerController = Cast<AABPlayerController>(It->Get());
		if (ABPlayerController && ScoredPlayer == ABPlayerController)
		{
			ABPlayerController->AddGameScore();
			break;
		}
	}

	ABGameState->AddGameScore();

	if (GetScore() >= ScoreToClear)
	{
		ABGameState->SetGameCleared();

		for (TActorIterator<APawn> It(GetWorld()); It; ++It)
		{
			(*It)->TurnOff();
		}

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			AABPlayerController* ABPlayerController = Cast<AABPlayerController>(It->Get());
			if (ABPlayerController)
			{
				ABPlayerController->ShowResultUI();
			}
		}
	}
}

int32 AABGameMode::GetScore() const
{
	return ABGameState ? ABGameState->GetTotalGameScore() : 0;
}
