// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "AIController.h"
#include "ABAIController.generated.h"

class UBlackboardData;
class UBehaviorTree;
/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AABAIController();

private:
	virtual void OnPossess(APawn* InPawn) override;

public:
	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;

private:
	UPROPERTY()
	UBlackboardData* BBAsset;

	UPROPERTY()
	UBehaviorTree* BTAsset;
};
