// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ABCharacterSetting.generated.h"

/**
 * 
 */
UCLASS(config=ArenaBattle)
class ARENABATTLESETTING_API UABCharacterSetting : public UObject
{
	GENERATED_BODY()
	
private:
	UABCharacterSetting();

public:
	UPROPERTY(config)
	TArray<FSoftObjectPath> CharacterAssets;
};
