// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Actor.h"
#include "ABItemBox.generated.h"

class AABWeapon;

UCLASS()
class ARENABATTLE_API AABItemBox : public AActor
{
	GENERATED_BODY()
	
private:
	AABItemBox();

private:
	virtual void PostInitializeComponents() final;

private:
	UFUNCTION()
	void OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEffectFinished(UParticleSystemComponent* PSystem);

private:
	UPROPERTY(VisibleAnywhere, Category = "Box")
	UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere, Category = "Box")
	UStaticMeshComponent* Box;

	UPROPERTY(EditInstanceOnly, Category = "Effect")
	UParticleSystemComponent* Effect;

	UPROPERTY(EditInstanceOnly, Category = "Box")
	TSubclassOf<AABWeapon> WeaponItemClass;
};
