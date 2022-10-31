// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HologateCharacter.generated.h"

UCLASS()
class HOLOGATEBLANK_API AHologateCharacter : public ACharacter
{
	GENERATED_BODY()


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	// Sets default values for this character's properties
	AHologateCharacter();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, Category = InitialValues)
		FVector InitialTransform;

	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category = "Health")
		void SetCurrentHealth(float healthValue);

	UFUNCTION(BlueprintCallable, Category = "Health")
		float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BluePrintImplementableEvent, Category = "Health")
		void UpdateWidget(float health);

	UFUNCTION(BlueprintCallable, Category = "Health")
		void Respawn();

protected:

	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
		float CurrentHealth;

	UFUNCTION()
		void OnRep_CurrentHealth();

	void OnHealthUpdate();

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|Projectile")
		TSubclassOf<class AHologateProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
		float FireRate;

	bool bIsFiringWeapon;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StopFire();

	UFUNCTION(Server, Reliable)
		void HandleFire();


	FTimerHandle FiringTimer;

protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
