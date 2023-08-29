// Fill out your copyright notice in the Description page of Project Settings.


#include "ABWeapon.h"

AABWeapon::AABWeapon()
	: AttackRange(150.f)
	, AttackDamageMin(-2.5f)
	, AttackDamageMax(10.f)
	, AttackModifierMin(0.85f)
	, AttackModifierMax(1.25f)
{
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
	RootComponent = Weapon;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(TEXT("/Game/InfinityBladeWeapons/Weapons/Blade/Swords/Blade_BlackKnight/SK_Blade_BlackKnight.SK_Blade_BlackKnight"));
	if (SK_WEAPON.Succeeded())
	{
		Weapon->SetSkeletalMesh(SK_WEAPON.Object);
	}

	Weapon->SetCollisionProfileName(TEXT("NoCollision"));
}

void AABWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	AttackDamage = FMath::RandRange(AttackDamageMin, AttackDamageMax);
	AttackModifier = FMath::RandRange(AttackModifierMin, AttackModifierMax);
	ABLOG(Warning, TEXT("Weapon Damage : %f, Modifier : %f"), AttackDamage, AttackModifier);
}

const float AABWeapon::GetAttackRange() const
{
	return AttackRange;
}

const float AABWeapon::GetAttackDamage() const
{
	return AttackDamage;
}

const float AABWeapon::GetAttackModifier() const
{
	return AttackModifier;
}
