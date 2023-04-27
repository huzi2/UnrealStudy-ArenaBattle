// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"

class UABHUDWidget;
class AABCharacter;
class AABPlayerState;
/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AABPlayerController();
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* aPawn) override;

public:
	UABHUDWidget* GetHUDWidget() const;
	void NPCKill(AABCharacter* KilledNPC) const;
	void AddGameScore() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<UABHUDWidget> HUDWidgetClass;

private:
	UPROPERTY()
	UABHUDWidget* HUDWidget;

	UPROPERTY()
	AABPlayerState* ABPlayerState;
};
