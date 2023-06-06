// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerState.h"
#include "ABGameInstance.h"
#include "ABSaveGame.h"

AABPlayerState::AABPlayerState()
	: SaveSlotName(TEXT("Player1"))
	, GameScore(0)
	, GameHighScore(0)
	, CharacterLevel(1)
	, Exp(0)
	, CharacterIndex(0)
{
}

int32 AABPlayerState::GetGameScore() const
{
	return GameScore;
}

int32 AABPlayerState::GetGameHighScore() const
{
	return GameHighScore;
}

int32 AABPlayerState::GetCharacterLevel() const
{
	return CharacterLevel;
}

int32 AABPlayerState::GetCharacterIndex() const
{
	return CharacterIndex;
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
	SavePlayerData();
	return DidLevelUp;
}

void AABPlayerState::AddGameScore()
{
	++GameScore;
	if (GameScore >= GameHighScore)
		GameHighScore = GameScore;

	OnPlayerStateChanged.Broadcast();
	SavePlayerData();
}

void AABPlayerState::InitPlayerData()
{
	auto ABSaveGame = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	if (!ABSaveGame)
		ABSaveGame = GetMutableDefault<UABSaveGame>();

	SetPlayerName(ABSaveGame->PlayerName);
	SetCharacterLevel(ABSaveGame->Level);
	GameScore = 0;
	GameHighScore = ABSaveGame->HighScore;
	Exp = ABSaveGame->Exp;
	CharacterIndex = ABSaveGame->CharacterIndex;
	SavePlayerData();
}

void AABPlayerState::SavePlayerData()
{
	UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();
	NewPlayerData->PlayerName = GetPlayerName();
	NewPlayerData->Level = CharacterLevel;
	NewPlayerData->Exp = Exp;
	NewPlayerData->HighScore = GameHighScore;
	NewPlayerData->CharacterIndex = CharacterIndex;

	if (!UGameplayStatics::SaveGameToSlot(NewPlayerData, SaveSlotName, 0))
		ABLOG(Error, TEXT("SaveGame Error!"));
}

void AABPlayerState::SetCharacterLevel(int32 NewCharacterLevel)
{
	auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(ABGameInstance);

	CurrentStatData = ABGameInstance->GetABCharacterData(NewCharacterLevel);
	ABCHECK(CurrentStatData);

	CharacterLevel = NewCharacterLevel;
}