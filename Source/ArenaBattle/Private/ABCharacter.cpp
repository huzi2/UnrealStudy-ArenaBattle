// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacter.h"
#include "VisualLogger/VisualLogger.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "ABAnimInstance.h"
#include "ABWeapon.h"
#include "ABCharacterStatComponent.h"
#include "ABCharacterWidget.h"
#include "ABAIController.h"
#include "ABGameInstance.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABHUDWidget.h"
#include "ABGameMode.h"
#include "ABCharacterSetting.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"

AABCharacter::AABCharacter()
	: IsAttacking(false)
	, CanNextCombo(false)
	, IsComboInputOn(false)
	, CurrentCombo(0)
	, MaxCombo(4)
	, AttackRange(80.f)
	, AttackRadius(50.f)
	, DeadTimer(5.f)
	, CurrentControlMode(EControlMode::DIABLO)
	, DirectionToMove(FVector::ZeroVector)
	, ArmLengthTo(0.f)
	, ArmRotationTo(FRotator::ZeroRotator)
	, ArmLengthSpeed(3.f)
	, ArmRotationSpeed(10.f)
	, CharacterAssetToLoad(FSoftObjectPath(nullptr))
	, AssetIndex(4)
{
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
	{
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Book/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C"));
	if (WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_DEFAULT(TEXT("/Game/ThirdPerson/Input/IMC_Default.IMC_Default"));
	if (IMC_DEFAULT.Succeeded())
	{
		DefaultMappingContext = IMC_DEFAULT.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_MOVE(TEXT("/Game/ThirdPerson/Input/Actions/IA_Move.IA_Move"));
	if (IA_MOVE.Succeeded())
	{
		MoveAction = IA_MOVE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_LOOK(TEXT("/Game/ThirdPerson/Input/Actions/IA_Look.IA_Look"));
	if (IA_LOOK.Succeeded())
	{
		LookAction = IA_LOOK.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_VIEWCHANGE(TEXT("/Game/ThirdPerson/Input/Actions/IA_ViewChange.IA_ViewChange"));
	if (IA_VIEWCHANGE.Succeeded())
	{
		ViewChangeAction = IA_VIEWCHANGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_JUMP(TEXT("/Game/ThirdPerson/Input/Actions/IA_Jump.IA_Jump"));
	if (IA_JUMP.Succeeded())
	{
		JumpAction = IA_JUMP.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_ATTACK(TEXT("/Game/ThirdPerson/Input/Actions/IA_Attack.IA_Attack"));
	if (IA_ATTACK.Succeeded())
	{
		AttackAction = IA_ATTACK.Object;
	}

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

	const UABCharacterSetting* DefaultSetting = GetDefault<UABCharacterSetting>();
	if (DefaultSetting->CharacterAssets.Num() > 0)
	{
		for (auto CharacterAsset : DefaultSetting->CharacterAssets)
		{
			ABLOG(Warning, TEXT("Character Asset : %s"), *CharacterAsset.ToString());
		}
	}

	SetActorHiddenInGame(true);
	HPBarWidget->SetHiddenInGame(true);
	SetCanBeDamaged(false);
}

void AABCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	bIsPlayer = IsPlayerControlled();
	if (bIsPlayer)
	{
		ABPlayerController = Cast<AABPlayerController>(GetController());
		ABCHECK(ABPlayerController);
	}
	else
	{
		ABAIController = Cast<AABAIController>(GetController());
		ABCHECK(ABAIController);
	}

	const UABCharacterSetting* DefaultSetting = GetDefault<UABCharacterSetting>();
	if (bIsPlayer)
	{
		AABPlayerState* ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
		ABCHECK(ABPlayerState);
		AssetIndex = ABPlayerState->GetCharacterIndex();
	}
	else
	{
		AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
	}

	CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex];
	UABGameInstance* ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(ABGameInstance);
	AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad, FStreamableDelegate::CreateUObject(this, &AABCharacter::OnAssetLoadCompleted));
	SetCharacterState(ECharacterState::LOADING);
}

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
	ABAnim->OnNextAttackCheck.AddLambda([&]()
		{
			ABLOG(Warning, TEXT("OnNextAttackCheck"));
			CanNextCombo = false;

			if (IsComboInputOn)
			{
				AttackStartComboState();
				ABAnim->JumpToAttackMontageSection(CurrentCombo);
			}
		});

	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck);
}

float AABCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

	CharacterStat->SetDamage(FinalDamage);
	if (CurrentState == ECharacterState::DEAD)
	{
		if (EventInstigator->IsPlayerController())
		{
			AABPlayerController* EventInstigatorController = Cast<AABPlayerController>(EventInstigator);
			ABCHECK(EventInstigatorController, 0.f);
			EventInstigatorController->NPCKill(this);
		}
	}

	return FinalDamage;
}

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

void AABCharacter::SetWeapon(AABWeapon* NewWeapon)
{
	ABCHECK(NewWeapon);

	if (CurrentWeapon)
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}
	
	if (NewWeapon)
	{
		FName WeaponSocket(TEXT("hand_rSocket"));
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
		{
			IsComboInputOn = true;
		}
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

void AABCharacter::SetCharacterState(const ECharacterState NewState)
{
	ABCHECK(CurrentState != NewState);
	CurrentState = NewState;

	switch (CurrentState)
	{
	case ECharacterState::LOADING:
		if (bIsPlayer)
		{
			DisableInput(ABPlayerController);

			ABPlayerController->GetHUDWidget()->BindCharacterStat(CharacterStat);

			AABPlayerState* ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
			ABCHECK(ABPlayerState);
			CharacterStat->SetNewLevel(ABPlayerState->GetCharacterLevel());
		}
		else
		{
			AABGameMode* ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
			ABCHECK(ABGameMode);
			int32 TargetLevel = FMath::CeilToInt(static_cast<float>(ABGameMode->GetScore()) * 0.8f);
			int32 FinalLevel = FMath::Clamp<int32>(TargetLevel, 1, 20);
			ABLOG(Warning, TEXT("New NPC Level : %d"), FinalLevel);
			CharacterStat->SetNewLevel(FinalLevel);
		}

		SetActorHiddenInGame(true);
		HPBarWidget->SetHiddenInGame(true);
		SetCanBeDamaged(false);
		break;
	case ECharacterState::READY:
	{
		SetActorHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(false);
		SetCanBeDamaged(true);

		CharacterStat->OnHPIsZero.AddLambda([this]()
			{
				SetCharacterState(ECharacterState::DEAD);
			});

		UABCharacterWidget* CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
		ABCHECK(CharacterWidget);
		CharacterWidget->BindCharacterStat(CharacterStat);

		if (bIsPlayer)
		{
			SetControlMode(EControlMode::DIABLO);
			GetCharacterMovement()->MaxWalkSpeed = 600.f;
			EnableInput(ABPlayerController);
		}
		else
		{
			SetControlMode(EControlMode::NPC);
			GetCharacterMovement()->MaxWalkSpeed = 400.f;
			ABAIController->RunAI();
		}
		break;
	}
	case ECharacterState::DEAD:
		SetActorEnableCollision(false);
		GetMesh()->SetHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(true);
		ABAnim->SetDeadAnim();
		SetCanBeDamaged(false);

		bIsPlayer ? DisableInput(ABPlayerController) : ABAIController->StopAI();
		GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]()
			{
				bIsPlayer ? ABPlayerController->ShowResultUI() : Destroy();
			}), DeadTimer, false);
		break;
	default:
		break;
	}
}

int32 AABCharacter::GetExp() const
{
	return CharacterStat ? CharacterStat->GetDropExp() : 0;
}

float AABCharacter::GetFinalAttackRange() const
{
	return CurrentWeapon ? CurrentWeapon->GetAttackRange() : AttackRange;
}

float AABCharacter::GetFinalAttackDamage() const
{
	const float AttackDamage = CurrentWeapon ? CharacterStat->GetAttack() + CurrentWeapon->GetAttackDamage() : CharacterStat->GetAttack();
	const float AttackModifier  = CurrentWeapon ? CurrentWeapon->GetAttackModifier() : 1.f;
	return AttackDamage * AttackModifier;
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

void AABCharacter::SetControlMode(const EControlMode NewControlMode)
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
	const float FinalAttackRange = GetFinalAttackRange();

	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	const bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * FinalAttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

#if ENABLE_DRAW_DEBUG

	const FVector TraceVec = GetActorForwardVector() * FinalAttackRange;
	const FVector Center = GetActorLocation() + TraceVec * 0.5f;
	const float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;
	const FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	const FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	const float DebugLifeTime = 5.f;

	DrawDebugCapsule(GetWorld(), Center, HalfHeight, AttackRadius, CapsuleRot, DrawColor, false, DebugLifeTime);

	UE_VLOG_LOCATION(this, ArenaBattle, Verbose, GetActorLocation(), 50.f, FColor::Blue, TEXT("Attack Position"));
	UE_VLOG_CAPSULE(this, ArenaBattle, Verbose, GetActorLocation() - GetActorForwardVector() * AttackRadius, HalfHeight, AttackRadius, CapsuleRot, DrawColor, TEXT("Attack Area"));

#endif

	if (bResult)
	{
		if (HitResult.GetActor()->IsValidLowLevel())
		{
			ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.GetActor()->GetName());

			FDamageEvent DamageEvent;
			HitResult.GetActor()->TakeDamage(GetFinalAttackDamage(), DamageEvent, GetController(), this);
		}
	}
}

void AABCharacter::OnAssetLoadCompleted()
{
	AssetStreamingHandle->ReleaseHandle();
	TSoftObjectPtr<USkeletalMesh> LoadedAssetPath(CharacterAssetToLoad);
	ABCHECK(LoadedAssetPath.IsValid());

	GetMesh()->SetSkeletalMesh(LoadedAssetPath.Get());
	SetCharacterState(ECharacterState::READY);
}
