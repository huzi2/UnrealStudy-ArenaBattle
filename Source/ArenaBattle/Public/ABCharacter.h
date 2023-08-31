// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Character.h"
#include "ABCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UABAnimInstance;
class AABWeapon;
class UABCharacterStatComponent;
class UWidgetComponent;
class AABAIController;
class AABPlayerController;
struct FInputActionValue;
struct FStreamableHandle;

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class ARENABATTLE_API AABCharacter : public ACharacter
{
	GENERATED_BODY()

	enum class EControlMode
	{
		GTA,
		DIABLO,
		NPC
	};

private:
	AABCharacter();

private:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	void SetWeapon(AABWeapon* NewWeapon);
	void Attack();
	void SetCharacterState(const ECharacterState NewState);
	FORCEINLINE ECharacterState GetCharacterState() const { return CurrentState; }
	int32 GetExp() const;
	float GetFinalAttackRange() const;
	float GetFinalAttackDamage() const;

private:
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void ViewChange();

	void SetControlMode(const EControlMode NewControlMode);

	void AttackStartComboState();
	void AttackEndComboState();
	void AttackCheck();

	void OnAssetLoadCompleted();

public:
	FOnAttackEndDelegate OnAttackEnd;

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = UI)
	UWidgetComponent* HPBarWidget;

	UPROPERTY(VisibleAnywhere, Category = Input)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(VisibleAnywhere, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	UInputAction* ViewChangeAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	UInputAction* AttackAction;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AABWeapon* CurrentWeapon;

	UPROPERTY(VisibleAnywhere, Category = Stat)
	UABCharacterStatComponent* CharacterStat;

	UPROPERTY()
	UABAnimInstance* ABAnim;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	bool CanNextCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	int32 MaxCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	float AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	float AttackRadius;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "State", Meta = (AllowPrivateAccess = true))
	ECharacterState CurrentState;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "State", Meta = (AllowPrivateAccess = true))
	bool bIsPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State", Meta = (AllowPrivateAccess = true))
	float DeadTimer;

	UPROPERTY()
	AABAIController* ABAIController;

	UPROPERTY()
	AABPlayerController* ABPlayerController;

private:
	EControlMode CurrentControlMode;
	FVector DirectionToMove;

	float ArmLengthTo;
	FRotator ArmRotationTo;
	float ArmLengthSpeed;
	float ArmRotationSpeed;

	FSoftObjectPath CharacterAssetToLoad;
	TSharedPtr<FStreamableHandle> AssetStreamingHandle;

	int32 AssetIndex;

	FTimerHandle DeadTimerHandle;
};
