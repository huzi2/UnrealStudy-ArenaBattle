// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Blueprint/UserWidget.h"
#include "ABHUDWidget.generated.h"

class UABCharacterStatComponent;
class AABPlayerState;
class UProgressBar;
class UTextBlock;
/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	virtual void NativeConstruct() final;

public:
	void BindCharacterStat(UABCharacterStatComponent* CharacterStat);
	void BindPlayerState(AABPlayerState* PlayerState);

private:
	void UpdateCharacterStat();
	void UpdatePlayerState();

private:
	UPROPERTY()
	UProgressBar* HPBar;

	UPROPERTY()
	UProgressBar* ExpBar;

	UPROPERTY()
	UTextBlock* PlayerName;

	UPROPERTY()
	UTextBlock* PlayerLevel;

	UPROPERTY()
	UTextBlock* CurrentScore;

	UPROPERTY()
	UTextBlock* HighScore;

private:
	TWeakObjectPtr<UABCharacterStatComponent> CurrentCharacterStat;
	TWeakObjectPtr<AABPlayerState> CurrentPlayerState;
};
