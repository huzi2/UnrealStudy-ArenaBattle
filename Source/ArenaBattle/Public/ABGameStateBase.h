// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/GameStateBase.h"
#include "ABGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
private:
	AABGameStateBase();

public:
	const int32 GetTotalGameScore() const;
	void AddGameScore();

	void SetGameCleared();
	const bool IsGameCleared() const;

private:
	UPROPERTY(Transient)
	int32 TotalGameScore;

	UPROPERTY(Transient)
	bool bGameCleared;
};
