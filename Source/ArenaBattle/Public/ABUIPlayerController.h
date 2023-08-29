// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/PlayerController.h"
#include "ABUIPlayerController.generated.h"

class UUserWidget;
/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABUIPlayerController : public APlayerController
{
	GENERATED_BODY()
	
private:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> UIWidgetClass;

private:
	UPROPERTY()
	UUserWidget* UIWidgetInstance;
};
