// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindPatrolPos.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UBTTask_FindPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindPatrolPos();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
