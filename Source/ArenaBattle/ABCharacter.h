// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Character.h"
#include "ABCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UABAnimInstance;
class AABWeapon;
struct FInputActionValue;

UCLASS()
class ARENABATTLE_API AABCharacter : public ACharacter
{
	GENERATED_BODY()

	enum class EControlMode
	{
		GTA,
		DIABLO
	};

public:
	// Sets default values for this character's properties
	AABCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	bool CanSetWeapon();
	void SetWeapon(AABWeapon* NewWeapon);

private:
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void ViewChange();
	void Attack();

	void SetControlMode(EControlMode NewControlMode);

	void AttackStartComboState();
	void AttackEndComboState();
	void AttackCheck();

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

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

	UPROPERTY()
	UABAnimInstance* ABAnim;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRadius;

private:
	EControlMode CurrentControlMode;
	FVector DirectionToMove;

	float ArmLengthTo;
	FRotator ArmRotationTo;
	float ArmLengthSpeed;
	float ArmRotationSpeed;
};
