// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameStateBase.h"

AABGameStateBase::AABGameStateBase()
	: TotalGameScore(0)
	, bGameCleared(false)
{
}

int32 AABGameStateBase::GetTotalGameScore() const
{
	return TotalGameScore;
}

void AABGameStateBase::AddGameScore()
{
	++TotalGameScore;
}

void AABGameStateBase::SetGameCleared()
{
	bGameCleared = true;
}

bool AABGameStateBase::IsGameCleared() const
{
	return bGameCleared;
}
