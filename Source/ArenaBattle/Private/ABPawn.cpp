// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

// Sets default values
AABPawn::AABPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CAPSULE"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));
	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	RootComponent = Capsule;
	Mesh->SetupAttachment(Capsule);
	SpringArm->SetupAttachment(Capsule);
	Camera->SetupAttachment(SpringArm);

	Capsule->SetCapsuleHalfHeight(88.f);
	Capsule->SetCapsuleRadius(34.f);
	Mesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if (SK_CARDBOARD.Succeeded())
		Mesh->SetSkeletalMesh(SK_CARDBOARD.Object);

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_DEFAULT(TEXT("/Game/ThirdPerson/Input/IMC_Default.IMC_Default"));
	if (IMC_DEFAULT.Succeeded())
		DefaultMappingContext = IMC_DEFAULT.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_MOVE(TEXT("/Game/ThirdPerson/Input/Actions/IA_Move.IA_Move"));
	if (IA_MOVE.Succeeded())
		MoveAction = IA_MOVE.Object;

	Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Book/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C"));
	if (WARRIOR_ANIM.Succeeded())
		Mesh->SetAnimInstanceClass(WARRIOR_ANIM.Class);
}

// Called when the game starts or when spawned
void AABPawn::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = CastChecked<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

// Called every frame
void AABPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AABPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}

void AABPawn::PossessedBy(AController* NewController)
{
	ABLOG_S(Warning);
	Super::PossessedBy(NewController);
}

// Called to bind functionality to input
void AABPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AABPawn::Move);
		////Jumping
		//Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		//Input->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		////Moving
		//Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AtestCharacter::Move);

		////Looking
		//Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &AtestCharacter::Look);
	}
}

void AABPawn::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	ABLOG(Warning, TEXT("%f, %f"), MovementVector.X, MovementVector.Y);

	if (Controller)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}
