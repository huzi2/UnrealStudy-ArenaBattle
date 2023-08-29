// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameplayResultWidget.h"
#include "ABGameStateBase.h"
#include "Components/TextBlock.h"

void UABGameplayResultWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ABCHECK(CurrentGameState.IsValid());

	UTextBlock* Result = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtResult")));
	ABCHECK(Result);
	Result->SetText(FText::FromString(CurrentGameState->IsGameCleared() ? TEXT("Mission Complete") : TEXT("Mission Failed")));

	UTextBlock* TotalScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtTotalScore")));
	ABCHECK(TotalScore);
	TotalScore->SetText(FText::FromString(FString::FromInt(CurrentGameState->GetTotalGameScore())));
}

void UABGameplayResultWidget::BindGameState(AABGameStateBase* GameState)
{
	ABCHECK(GameState);
	CurrentGameState = GameState;
}
