// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"

class UABHUDWidget;
class AABCharacter;
class AABPlayerState;
class UInputMappingContext;
class UInputAction;
class UABGameplayWidget;
class UABGameplayResultWidget;
/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	AABPlayerController();
	
private:
	virtual void BeginPlay() final;
	virtual void SetupInputComponent() final;

public:
	FORCEINLINE UABHUDWidget* GetHUDWidget() const { return HUDWidget; }
	void NPCKill(AABCharacter* KilledNPC) const;
	void AddGameScore() const;
	void ChangeInputMode(const bool bGameMode = true);
	void ShowResultUI();

private:
	void OnGamePause();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UABHUDWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UABGameplayWidget> MenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UABGameplayResultWidget> ResultWidgetClass;

private:
	UPROPERTY()
	UABHUDWidget* HUDWidget;

	UPROPERTY()
	UABGameplayWidget* MenuWidget;

	UPROPERTY()
	UABGameplayResultWidget* ResultWidget;

	UPROPERTY()
	AABPlayerState* ABPlayerState;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(VisibleAnywhere, Category = "Input")
	UInputAction* GmaePauseAction;

private:
	FInputModeGameOnly GameInputMode;
	FInputModeUIOnly UIInputMode;
};
