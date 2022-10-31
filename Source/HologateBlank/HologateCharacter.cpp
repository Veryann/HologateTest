// Fill out your copyright notice in the Description page of Project Settings.



#include "HologateCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "HologateProjectile.h"
#include "TimerManager.h"


// Sets default values
AHologateCharacter::AHologateCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 600.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 150.0f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	ProjectileClass = AHologateProjectile::StaticClass();
	FireRate = 0.25f;
	bIsFiringWeapon = false;
}

void AHologateCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AHologateCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHologateCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AHologateCharacter::StartFire);

}

// Called when the game starts or when spawned
void AHologateCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitialTransform = GetActorLocation();
	
}

// Called every frame
void AHologateCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHologateCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AHologateCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AHologateCharacter::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHologateCharacter, CurrentHealth);
}

void AHologateCharacter::OnHealthUpdate()
{
	if (CurrentHealth <= 0)
	{
		Respawn();
	}
	UpdateWidget(CurrentHealth);
}

void AHologateCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void AHologateCharacter::SetCurrentHealth(float healthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
		OnHealthUpdate();
	}
}

float AHologateCharacter::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float damageApplied = CurrentHealth - DamageTaken;
	SetCurrentHealth(damageApplied);
	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Take Damage %f"), CurrentHealth));
	}
	return damageApplied;
}

void AHologateCharacter::StartFire()
{
	if (!bIsFiringWeapon)
	{
		bIsFiringWeapon = true;
		UWorld* World = GetWorld();
		World->GetTimerManager().SetTimer(FiringTimer, this, &AHologateCharacter::StopFire, FireRate, false);
		HandleFire();
	}
}

void AHologateCharacter::StopFire()
{
	bIsFiringWeapon = false;
}

void AHologateCharacter::HandleFire_Implementation()
{
	FVector spawnLocation = GetActorLocation() + (GetControlRotation().Vector() * 100.0f) + (GetActorUpVector() * 50.0f);
	FRotator spawnRotation = GetControlRotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;

	AHologateProjectile* spawnedProjectile = GetWorld()->SpawnActor<AHologateProjectile>(spawnLocation, spawnRotation, spawnParameters);
}

void AHologateCharacter::Respawn()
{
	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("RespawnCalled %s"), *InitialTransform.ToString()));
	}

	SetActorLocation(InitialTransform, false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
	CurrentHealth = 100.f;
}
