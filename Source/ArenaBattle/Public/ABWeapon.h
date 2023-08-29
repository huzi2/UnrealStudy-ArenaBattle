// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Actor.h"
#include "ABWeapon.generated.h"

UCLASS()
class ARENABATTLE_API AABWeapon : public AActor
{
	GENERATED_BODY()
	
private:
	AABWeapon();

private:
	virtual void BeginPlay() override;

public:
	const float GetAttackRange() const;
	const float GetAttackDamage() const;
	const float GetAttackModifier() const;

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack")
	float AttackDamage;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack")
	float AttackModifier;

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	USkeletalMeshComponent* Weapon;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackRange;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackDamageMin;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackDamageMax;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackModifierMin;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackModifierMax;
};
