// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "ABAIController.h"
#include "ABCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("Turn");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NdoeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NdoeMemory);

	AABCharacter* ABCharacter = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!ABCharacter) return EBTNodeResult::Failed;

	AABCharacter* Target = Cast<AABCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey));
	if(!Target) return EBTNodeResult::Failed;

	FVector LookVector = Target->GetActorLocation() - ABCharacter->GetActorLocation();
	LookVector.Z = 0.f;
	const FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	ABCharacter->SetActorRotation(FMath::RInterpTo(ABCharacter->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 2.f));

	return EBTNodeResult::Succeeded;
}
