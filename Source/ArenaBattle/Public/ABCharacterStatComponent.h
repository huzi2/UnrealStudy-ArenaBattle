// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Components/ActorComponent.h"
#include "ABCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPIsZeroDelegate);
DECLARE_MULTICAST_DELEGATE(FOnHPChangeDelegate);

struct FABCharacterData;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UABCharacterStatComponent();

private:
	virtual void InitializeComponent() final;

public:
	void SetNewLevel(const int32 NewLevel);
	void SetDamage(const float NewDamage);
	void SetHP(const float NewHP);
	float GetAttack() const;
	float GetHPRatio() const;
	int32 GetDropExp() const;

private:
	UPROPERTY(EditInstanceOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	int32 Level;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = "Stat", Meta = (AllowPrivateAccess = true))
	float CurrentHP;

public:
	FOnHPIsZeroDelegate OnHPIsZero;
	FOnHPChangeDelegate OnHPChanged;

private:
	FABCharacterData* CurrentStatData;
};
