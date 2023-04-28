// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Blueprint/UserWidget.h"
#include "ABCharacterSelectWidget.generated.h"

class UButton;
class UEditableTextBox;
/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABCharacterSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	virtual void NativeConstruct() override;

private:
	UFUNCTION(BlueprintCallable)
	void NextCharacter(bool bForward = true);

	UFUNCTION()
	void OnPrevClicked();

	UFUNCTION()
	void OnNextClicked();

	UFUNCTION()
	void OnConfirmClicked();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	int32 CurrentIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	int32 MaxIndex;

private:
	UPROPERTY()
	UButton* PrevButton;

	UPROPERTY()
	UButton* NextButton;

	UPROPERTY()
	UEditableTextBox* TextBox;

	UPROPERTY()
	UButton* ConfirmButton;

private:
	TWeakObjectPtr<USkeletalMeshComponent> TargetComponent;
};
