// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerState.h"
#include "ABGameInstance.h"

AABPlayerState::AABPlayerState()
	: GameScore(0)
	, CharacterLevel(1)
	, Exp(0)
{
}

int32 AABPlayerState::GetGameScore() const
{
	return GameScore;
}

int32 AABPlayerState::GetCharacterLevel() const
{
	return CharacterLevel;
}

float AABPlayerState::GetExpRatio() const
{
	if (CurrentStatData->NextExp <= KINDA_SMALL_NUMBER)
		return 0.f;
	
	float Result = static_cast<float>(Exp) / static_cast<float>(CurrentStatData->NextExp);
	ABLOG(Warning, TEXT("Ratio : %f, Current : %d, Next : %d"), Result, Exp, CurrentStatData->NextExp);
	return Result;
}

bool AABPlayerState::AddExp(int32 IncomeExp)
{
	if (CurrentStatData->NextExp == -1)
		return false;

	bool DidLevelUp = false;
	Exp += IncomeExp;
	if (Exp >= CurrentStatData->NextExp)
	{
		Exp -= CurrentStatData->NextExp;
		SetCharacterLevel(++CharacterLevel);
		DidLevelUp = true;
	}

	OnPlayerStateChanged.Broadcast();
	return DidLevelUp;
}

void AABPlayerState::AddGameScore()
{
	++GameScore;
	OnPlayerStateChanged.Broadcast();
}

void AABPlayerState::InitPlayerData()
{
	SetPlayerName(TEXT("Destiny"));
	SetCharacterLevel(5);
	GameScore = 0;
	Exp = 0;
}

void AABPlayerState::SetCharacterLevel(int32 NewCharacterLevel)
{
	auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(ABGameInstance);

	CurrentStatData = ABGameInstance->GetABCharacterData(NewCharacterLevel);
	ABCHECK(CurrentStatData);

	CharacterLevel = NewCharacterLevel;
}
