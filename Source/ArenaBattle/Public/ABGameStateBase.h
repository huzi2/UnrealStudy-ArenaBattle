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
	FORCEINLINE int32 GetTotalGameScore() const { return TotalGameScore; };
	FORCEINLINE void AddGameScore() { ++TotalGameScore; }

	FORCEINLINE void SetGameCleared() { bGameCleared = true; }
	FORCEINLINE bool IsGameCleared() const { return bGameCleared; }

private:
	UPROPERTY(Transient)
	int32 TotalGameScore;

	UPROPERTY(Transient)
	bool bGameCleared;
};
