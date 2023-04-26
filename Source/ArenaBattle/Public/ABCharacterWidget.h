// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Blueprint/UserWidget.h"
#include "ABCharacterWidget.generated.h"

class UABCharacterStatComponent;
class UProgressBar;

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABCharacterWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	virtual void NativeConstruct() override;

private:
	void UpdateHPWidget();

public:
	void BindCharacterStat(UABCharacterStatComponent* NewCharacterStat);

private:
	UPROPERTY()
	UProgressBar* HPProgressBar;

private:
	TWeakObjectPtr<UABCharacterStatComponent> CurrentCharacterStat;
};
