// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "ABAnimInstance.h"
#include "ABWeapon.h"
#include "ABCharacterStatComponent.h"
#include "ABCharacterWidget.h"
#include "ABAIController.h"
#include "ABGameInstance.h"
#include "ABCharacterSetting.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"

// Sets default values
AABCharacter::AABCharacter()
	: IsAttacking(false)
	, CanNextCombo(false)
	, IsComboInputOn(false)
	, CurrentCombo(0)
	, MaxCombo(4)
	, AttackRange(200.f)
	, AttackRadius(50.f)
	, CurrentControlMode(EControlMode::DIABLO)
	, DirectionToMove(FVector::ZeroVector)
	, ArmLengthTo(0.f)
	, ArmRotationTo(FRotator::ZeroRotator)
	, ArmLengthSpeed(3.f)
	, ArmRotationSpeed(10.f)
	, CharacterAssetToLoad(FSoftObjectPath(nullptr))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	CharacterStat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("CHARACTERSTAT"));
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
	HPBarWidget->SetupAttachment(GetMesh());

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if (SK_CARDBOARD.Succeeded())
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Book/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C"));
	if (WARRIOR_ANIM.Succeeded())
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_DEFAULT(TEXT("/Game/ThirdPerson/Input/IMC_Default.IMC_Default"));
	if (IMC_DEFAULT.Succeeded())
		DefaultMappingContext = IMC_DEFAULT.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_MOVE(TEXT("/Game/ThirdPerson/Input/Actions/IA_Move.IA_Move"));
	if (IA_MOVE.Succeeded())
		MoveAction = IA_MOVE.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_LOOK(TEXT("/Game/ThirdPerson/Input/Actions/IA_Look.IA_Look"));
	if (IA_LOOK.Succeeded())
		LookAction = IA_LOOK.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_VIEWCHANGE(TEXT("/Game/ThirdPerson/Input/Actions/IA_ViewChange.IA_ViewChange"));
	if (IA_VIEWCHANGE.Succeeded())
		ViewChangeAction = IA_VIEWCHANGE.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_JUMP(TEXT("/Game/ThirdPerson/Input/Actions/IA_Jump.IA_Jump"));
	if (IA_JUMP.Succeeded())
		JumpAction = IA_JUMP.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_ATTACK(TEXT("/Game/ThirdPerson/Input/Actions/IA_Attack.IA_Attack"));
	if (IA_ATTACK.Succeeded())
		AttackAction = IA_ATTACK.Object;

	SetControlMode(CurrentControlMode);

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));

	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("/Game/Book/UI/UI_HPBar.UI_HPBar_C"));
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.f, 50.f));
	}

	AIControllerClass = AABAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	auto DefaultSetting = GetDefault<UABCharacterSetting>();
	if (DefaultSetting->CharacterAssets.Num() > 0)
	{
		for (auto CharacterAsset : DefaultSetting->CharacterAssets)
		{
			ABLOG(Warning, TEXT("Character Asset : %s"), *CharacterAsset.ToString());
		}
	}
}

// Called when the game starts or when spawned
void AABCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
	if (CharacterWidget)
		CharacterWidget->BindCharacterStat(CharacterStat);

	if (!IsPlayerControlled())
	{
		auto DefaultSetting = GetDefault<UABCharacterSetting>();
		int32 RandIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
		CharacterAssetToLoad = DefaultSetting->CharacterAssets[RandIndex];

		auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
		if (ABGameInstance)
			AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad, FStreamableDelegate::CreateUObject(this, &AABCharacter::OnAssetLoadCompleted));
	}
}

// Called every frame
void AABCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	switch (CurrentControlMode)
	{
	case EControlMode::DIABLO:
		SpringArm->SetRelativeRotation(FMath::RInterpTo(SpringArm->GetRelativeRotation(), ArmRotationTo, DeltaTime, ArmRotationSpeed));

		if (DirectionToMove.SizeSquared() > 0.f)
		{
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
			AddMovementInput(DirectionToMove);

			DirectionToMove.X = 0.f;
			DirectionToMove.Y = 0.f;
		}
		break;
	default:
		break;
	}
}

void AABCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ABAnim = CastChecked<UABAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(ABAnim);

	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharacter::OnAttackMontageEnded);
	ABAnim->OnNextAttackCheck.AddLambda([this]() -> void {
			ABLOG(Warning, TEXT("OnNextAttackCheck"));
			CanNextCombo = false;

			if (IsComboInputOn)
			{
				AttackStartComboState();
				ABAnim->JumpToAttackMontageSection(CurrentCombo);
			}
		});

	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck);

	CharacterStat->OnHPIsZero.AddLambda([this]() -> void {
			ABLOG(Warning, TEXT("OnHPIsZero"));
			ABAnim->SetDeadAnim();
			SetActorEnableCollision(false);
		});
}

inline float AABCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

	CharacterStat->SetDamage(FinalDamage);

	return FinalDamage;
}

void AABCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (IsPlayerControlled())
	{
		SetControlMode(EControlMode::DIABLO);
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
	else
	{
		SetControlMode(EControlMode::NPC);
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
	}
}

// Called to bind functionality to input
void AABCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AABCharacter::Move);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AABCharacter::Look);
		Input->BindAction(ViewChangeAction, ETriggerEvent::Triggered, this, &AABCharacter::ViewChange);
		Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		Input->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AABCharacter::Attack);
	}
}

bool AABCharacter::CanSetWeapon()
{
	return (CurrentWeapon == nullptr);
}

void AABCharacter::SetWeapon(AABWeapon* NewWeapon)
{
	ABCHECK(NewWeapon && !CurrentWeapon);

	FName WeaponSocket(TEXT("hand_rSocket"));
	if (NewWeapon)
	{
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
}

void AABCharacter::Attack()
{
	if (IsAttacking)
	{
		ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		if (CanNextCombo)
			IsComboInputOn = true;
	}
	else
	{
		ABCHECK(CurrentCombo == 0);
		AttackStartComboState();
		ABAnim->PlayAttackMontage();
		ABAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}

void AABCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
	OnAttackEnd.Broadcast();
}

void AABCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), MovementVector.Y);
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), MovementVector.X);
		break;
	case EControlMode::DIABLO:
		DirectionToMove.X = MovementVector.Y;
		DirectionToMove.Y = MovementVector.X;
		break;
	default:
		break;
	}
}

void AABCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
		break;
	default:
		break;
	}
}

void AABCharacter::ViewChange()
{
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		GetController()->SetControlRotation(GetActorRotation());
		SetControlMode(EControlMode::DIABLO);
		break;
	case EControlMode::DIABLO:
		GetController()->SetControlRotation(SpringArm->GetRelativeRotation());
		SetControlMode(EControlMode::GTA);
		break;
	default:
		break;
	}
}

void AABCharacter::SetControlMode(EControlMode NewControlMode)
{
	CurrentControlMode = NewControlMode;

	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		//SpringArm->TargetArmLength = 450.f;
		//SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
		ArmLengthTo = 450.f;
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = true;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
		break;
	case EControlMode::DIABLO:
		//SpringArm->TargetArmLength = 800.f;
		//SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
		ArmLengthTo = 800.f;
		ArmRotationTo = FRotator(-45.f, 0.f, 0.f);
		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritRoll = false;
		SpringArm->bInheritYaw = false;
		SpringArm->bDoCollisionTest = false;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
		break;
	case EControlMode::NPC:
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);
		break;
	default:
		break;
	}
}

void AABCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AABCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AABCharacter::AttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

#if ENABLE_DRAW_DEBUG

	FVector TraceVec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.f;

	DrawDebugCapsule(GetWorld(), Center, HalfHeight, AttackRadius, CapsuleRot, DrawColor, false, DebugLifeTime);

#endif

	if (bResult)
	{
		if (HitResult.GetActor()->IsValidLowLevel())
		{
			ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.GetActor()->GetName());

			FDamageEvent DamageEvent;
			HitResult.GetActor()->TakeDamage(CharacterStat->GetAttack(), DamageEvent, GetController(), this);
		}
	}
}

void AABCharacter::OnAssetLoadCompleted()
{
	AssetStreamingHandle->ReleaseHandle();
	TSoftObjectPtr<USkeletalMesh> LoadedAssetPath(CharacterAssetToLoad);
	if (LoadedAssetPath.IsValid())
		GetMesh()->SetSkeletalMesh(LoadedAssetPath.Get());
}

