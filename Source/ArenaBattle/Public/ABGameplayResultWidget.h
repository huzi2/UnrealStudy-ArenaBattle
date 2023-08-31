// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "ABGameplayWidget.h"
#include "ABGameplayResultWidget.generated.h"

class AABGameStateBase;
/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABGameplayResultWidget : public UABGameplayWidget
{
	GENERATED_BODY()
	
private:
	virtual void NativeConstruct() final;

public:
	void BindGameState(AABGameStateBase* GameState);

private:
	TWeakObjectPtr<AABGameStateBase> CurrentGameState;
};
