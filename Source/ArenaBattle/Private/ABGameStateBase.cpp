// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameStateBase.h"

AABGameStateBase::AABGameStateBase()
	: TotalGameScore(0)
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
