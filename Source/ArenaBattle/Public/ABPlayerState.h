// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/PlayerState.h"
#include "ABPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnPlayerStateChangedDelegate);

struct FABCharacterData;
/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerState : public APlayerState
{
	GENERATED_BODY()
	
private:
	AABPlayerState();

public:
	const int32 GetGameScore() const;
	const int32 GetGameHighScore() const;
	const int32 GetCharacterLevel() const;
	const int32 GetCharacterIndex() const;
	const float GetExpRatio() const;
	const bool AddExp(const int32 IncomeExp);
	void AddGameScore();

	void InitPlayerData();
	void SavePlayerData();

private:
	void SetCharacterLevel(const int32 NewCharacterLevel);

public:
	FOnPlayerStateChangedDelegate OnPlayerStateChanged;
	FABCharacterData* CurrentStatData;
	FString SaveSlotName;

private:
	UPROPERTY(Transient)
	int32 GameScore;

	UPROPERTY(Transient)
	int32 GameHighScore;

	UPROPERTY(Transient)
	int32 CharacterLevel;

	UPROPERTY(Transient)
	int32 Exp;

	UPROPERTY(Transient)
	int32 CharacterIndex;
};
