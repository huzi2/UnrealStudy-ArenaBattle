// Fill out your copyright notice in the Description page of Project Settings.


#include "ABItemBox.h"
#include "ABWeapon.h"
#include "ABCharacter.h"

AABItemBox::AABItemBox()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BOX"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));

	RootComponent = Trigger;
	Box->SetupAttachment(RootComponent);
	Effect->SetupAttachment(RootComponent);

	Trigger->SetBoxExtent(FVector(40.f, 42.f, 30.f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BOX(TEXT("/Game/InfinityBladeGrassLands/Environments/Breakables/StaticMesh/Box/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1"));
	if (SM_BOX.Succeeded())
	{
		Box->SetStaticMesh(SM_BOX.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_CHESTOPEN(TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Treasure/Chest/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh"));
	if (P_CHESTOPEN.Succeeded())
	{
		Effect->SetTemplate(P_CHESTOPEN.Object);
		Effect->bAutoActivate = false;
	}

	Box->SetRelativeLocation(FVector(0.f, -3.5f, -30.f));

	Trigger->SetCollisionProfileName(TEXT("ItemBox"));
	Box->SetCollisionProfileName(TEXT("NoCollision"));

	WeaponItemClass = AABWeapon::StaticClass();
}

void AABItemBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABItemBox::OnCharacterOverlap);
}

void AABItemBox::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABLOG_S(Warning);

	AABCharacter* ABCharacter = Cast<AABCharacter>(OtherActor);
	ABCHECK(ABCharacter);

	if (ABCharacter && WeaponItemClass)
	{
		AABWeapon* NewWeapon = GetWorld()->SpawnActor<AABWeapon>(WeaponItemClass, FVector::ZeroVector, FRotator::ZeroRotator);
		ABCharacter->SetWeapon(NewWeapon);
		Effect->Activate(true);
		Box->SetHiddenInGame(true, true);
		SetActorEnableCollision(false);
		Effect->OnSystemFinished.AddDynamic(this, &AABItemBox::OnEffectFinished);
	}
}

void AABItemBox::OnEffectFinished(UParticleSystemComponent* PSystem)
{
	Destroy();
}
