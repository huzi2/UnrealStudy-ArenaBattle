// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/GameModeBase.h"
#include "ABGameMode.generated.h"

class AABPlayerController;
class AABGameStateBase;
/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABGameMode : public AGameModeBase
{
	GENERATED_BODY()

private:
	AABGameMode();

public:
	virtual void PostInitializeComponents() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

public:
	void AddScore(AABPlayerController* ScoredPlayer);
	const int32 GetScore() const;

private:
	UPROPERTY()
	AABGameStateBase* ABGameState;

private:
	int32 ScoreToClear;
};
