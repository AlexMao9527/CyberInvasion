// Fill out your copyright notice in the Description page of Project Settings.


#include "MAIShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CyberInvasion/CyberInvasion.h"
#include "Components/MHealthComponent.h"
#include "MRifle.h"

// Sets default values
AMAIShooterCharacter::AMAIShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	/*springArmComp = CreateDefaultSubobject< USpringArmComponent>(TEXT("Spring Arm Comp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->bUsePawnControlRotation = true;
	cameraComp = CreateDefaultSubobject< UCameraComponent>(TEXT("Camera Comp"));
	cameraComp->SetupAttachment(springArmComp);*/

	healthComponent = CreateDefaultSubobject< UMHealthComponent>(TEXT("Health Comp"));

	GetMovementComponent()->NavAgentProps.bCanCrouch = true;

	weaponSocketName = TEXT("weaponSocket");
	GetCapsuleComponent()->SetCollisionResponseToChannel(WEAPON_CHANNEL, ECR_Ignore);

	bIsDead = false;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;	//这是为了给新生成的AIPawn自动分配AIController
}

// Called when the game starts or when spawned
void AMAIShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	healthComponent->onHealthChanged.AddDynamic(this, &AMAIShooterCharacter::characterHealthChanged);

	if (GetLocalRole() == ROLE_Authority) {
		FActorSpawnParameters weaponSpawnParams;
		weaponSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		currentWeapon = GetWorld()->SpawnActor<AMRifle>(weaponClass, FVector::ZeroVector, FRotator::ZeroRotator, weaponSpawnParams);
		if (currentWeapon)
		{
			GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
			currentWeapon->SetActorRelativeLocation(FVector(0, 13, 2));
			currentWeapon->SetOwner(this);
			//FAttachmentTransformRules rules(EAttachmentRule::KeepRelative);
			currentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, weaponSocketName);
			currentWeapon->SetActorRelativeLocation(FVector(1.34, -11.16, -1.7));
		}
	}
}

// Called every frame
void AMAIShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMAIShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMAIShooterCharacter::moveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMAIShooterCharacter::moveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMAIShooterCharacter::lookUp);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &AMAIShooterCharacter::lookRight);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
//	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMAIShooterCharacter::startFire);
//	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMAIShooterCharacter::stopFire);
}

void AMAIShooterCharacter::moveForward(float axisValue)
{
	AddMovementInput(GetActorForwardVector() * axisValue);
}

void AMAIShooterCharacter::moveRight(float axisValue)
{
	AddMovementInput(GetActorRightVector() * axisValue);
}

void AMAIShooterCharacter::lookUp(float axisValue)
{
	AddControllerPitchInput(axisValue);
}

void AMAIShooterCharacter::lookRight(float axisValue)
{
	AddControllerYawInput(axisValue);
}

bool AMAIShooterCharacter::isDead()const
{
	return bIsDead;
}

float AMAIShooterCharacter::gethealthPercentage()const
{
	//UE_LOG(LogTemp, Warning, TEXT("curHealth percentage: %f"), healthComponent->getHealthPercentage());
	return healthComponent->getHealthPercentage();
}

void AMAIShooterCharacter::startFire()
{
	if (currentWeapon) {
		currentWeapon->startFire();
	}
}

void AMAIShooterCharacter::stopFire()
{
	if (currentWeapon) {
		currentWeapon->stopFire();
	}
}

void AMAIShooterCharacter::characterHealthChanged(UMHealthComponent* healthComp, float health, float healthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	/*UE_LOG(LogTemp, Warning, TEXT("maohaoran AMShooterCharacter::characterHealthChanged current health: %s"), *FString::SanitizeFloat(health));
	UE_LOG(LogTemp, Warning, TEXT("maohaoran ROLE: %d"), GetLocalRole());*/
	if (health <= 0.0f && !bIsDead) {
		bIsDead = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DetachFromControllerPendingDestroy();
		currentWeapon->SetLifeSpan(10);
		SetLifeSpan(10);
	}
}

//void AMAIShooterCharacter::OnRep_CurMaxWalkSpeed()
//{
//	OnSetMaxWalkSpeed(curMaxWalkSpeed);
//}

void AMAIShooterCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMAIShooterCharacter, currentWeapon);
	DOREPLIFETIME(AMAIShooterCharacter, bIsDead);
	//DOREPLIFETIME(AMAIShooterCharacter, curMaxWalkSpeed);
}