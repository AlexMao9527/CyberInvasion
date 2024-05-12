// Fill out your copyright notice in the Description page of Project Settings.


#include "MShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CyberInvasion/CyberInvasion.h"
#include "Components/MHealthComponent.h"
#include "MRifle.h"
#include "MMultiPlayerController.h"
#include "MPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "AMMultiPlayerGameMode.h"

// Sets default values
AMShooterCharacter::AMShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	springArmComp = CreateDefaultSubobject< USpringArmComponent>(TEXT("Spring Arm Comp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->bUsePawnControlRotation = true;
	cameraComp = CreateDefaultSubobject< UCameraComponent>(TEXT("Camera Comp"));
	cameraComp->SetupAttachment(springArmComp);

	healthComponent = CreateDefaultSubobject< UMHealthComponent>(TEXT("Health Comp"));

	GetMovementComponent()->NavAgentProps.bCanCrouch = true;

	zoomedFOV = 65.f;
	zoomInterSpeed = 20.f;
	wantToZoom = false;
	weaponSocketName = TEXT("weaponSocket");
	GetCapsuleComponent()->SetCollisionResponseToChannel(WEAPON_CHANNEL, ECR_Ignore);

	bIsDead = false;
	SetReplicateMovement(true);
	//AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;	//这是为了给新生成的AIPawn自动分配AIController

}

// Called when the game starts or when spawned
void AMShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	defaultFOV = cameraComp->FieldOfView;
	healthComponent->onHealthChanged.AddDynamic(this, &AMShooterCharacter::characterHealthChanged);

	if (GetLocalRole() == ROLE_Authority) {
		FActorSpawnParameters weaponSpawnParams;
		weaponSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		currentWeapon = GetWorld()->SpawnActor<AMRifle>(weaponClass, FVector::ZeroVector, FRotator::ZeroRotator, weaponSpawnParams);
		if (currentWeapon)
		{
			//UE_LOG(LogTemp, Warning, TEXT("maohaoran BeginPlay 60"));
			GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
			//currentWeapon->SetActorRelativeLocation(FVector(0, 13, 2));
			currentWeapon->SetOwner(this);
			//FAttachmentTransformRules rules(EAttachmentRule::KeepRelative);
			currentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, weaponSocketName);
			currentWeapon->SetActorRelativeLocation(FVector(1.34, -11.16, -1.7));
			//allWeapons.Add(currentWeapon);
		}
	}
}

// Called every frame
void AMShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float targetFOV = wantToZoom ? zoomedFOV : defaultFOV;

	float newFOV = FMath::FInterpTo(cameraComp->FieldOfView, targetFOV, DeltaTime, zoomInterSpeed);
	cameraComp->SetFieldOfView(newFOV);
}

// Called to bind functionality to input
void AMShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMShooterCharacter::moveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMShooterCharacter::moveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMShooterCharacter::lookUp);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &AMShooterCharacter::lookRight);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AMShooterCharacter::beginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AMShooterCharacter::endZoom);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMShooterCharacter::startFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMShooterCharacter::stopFire);
	//PlayerInputComponent->BindAction("ChangeWeapon", IE_Released, this, &AMShooterCharacter::changeWeapon);
}

void AMShooterCharacter::moveForward(float axisValue)
{
	AddMovementInput(GetActorForwardVector() * axisValue);
}

void AMShooterCharacter::moveRight(float axisValue)
{
	AddMovementInput(GetActorRightVector() * axisValue);
}

void AMShooterCharacter::lookUp(float axisValue)
{
	AddControllerPitchInput(axisValue);
}

void AMShooterCharacter::lookRight(float axisValue)
{
	AddControllerYawInput(axisValue);
}

bool AMShooterCharacter::isDead()const
{
	return bIsDead;
}

float AMShooterCharacter::gethealthPercentage()const
{
	//UE_LOG(LogTemp, Warning, TEXT("curHealth percentage: %f"), healthComponent->getHealthPercentage());
	return healthComponent->getHealthPercentage();
}

void AMShooterCharacter::startFire()
{
	if (currentWeapon) {
		currentWeapon->startFire();
	}
}

void AMShooterCharacter::stopFire()
{
	if (currentWeapon) {
		currentWeapon->stopFire();
	}
}

void AMShooterCharacter::beginZoom()
{
	wantToZoom = true;
}

void AMShooterCharacter::endZoom()
{
	wantToZoom = false;
}

void AMShooterCharacter::characterHealthChanged(UMHealthComponent* healthComp, float health, float healthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	//UE_LOG(LogTemp, Warning, TEXT("maohaoran AMShooterCharacter::characterHealthChanged current health: %s"), *FString::SanitizeFloat(health));
	//UE_LOG(LogTemp, Warning, TEXT("maohaoran ROLE: %d"), GetLocalRole());
	if (health <= 0.0f && !bIsDead) {
		bIsDead = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DetachFromControllerPendingDestroy();
		currentWeapon->SetLifeSpan(10);
		/*for (int i = 0; i < allWeapons.Num(); i++) {
			allWeapons[i]->SetLifeSpan(10);
		}*/
		SetLifeSpan(10);
	}
}

void AMShooterCharacter::OnRep_CurMaxWalkSpeed()
{
	//UE_LOG(LogTemp, Warning, TEXT("maohaoran OnRep_CurMaxWalkSpeed curMaxWalkSpeed: %f"), curMaxWalkSpeed);
	OnSetMaxWalkSpeed(curMaxWalkSpeed);
	//GetCharacterMovement()->MaxWalkSpeed
}

void AMShooterCharacter::netMulticastChangePlayerName_Implementation()
{
	onChangePlayerNameWidget();
}

void AMShooterCharacter::serverChangePlayerName_Implementation(const FString& playerName)
{
	AMPlayerState* playerState = Cast< AMPlayerState>(GetPlayerState());
	if (!playerState)
		return;

	playerState->SetPlayerName(playerName);
	//netMulticastChangePlayerName();
	AAMMultiPlayerGameMode* gameMode = Cast< AAMMultiPlayerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gameMode) {
		gameMode->refreshAllPlayerName();
	}
}

void AMShooterCharacter::setPlayerNameWidget(const FString& playerName)
{
	if (IsLocallyControlled()) {
		serverChangePlayerName(playerName);
	}
}

void AMShooterCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMShooterCharacter, currentWeapon);
	DOREPLIFETIME(AMShooterCharacter, bIsDead);
	DOREPLIFETIME(AMShooterCharacter, curMaxWalkSpeed);
}

void AMShooterCharacter::addNewWeapon(TSubclassOf<class AMRifle> newWeaponClass)
{
	//UE_LOG(LogTemp, Warning, TEXT("maohaoran addNewWeapon 213"));
	if (GetLocalRole() == ROLE_Authority) {

		FActorSpawnParameters weaponSpawnParams;
		weaponSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		AMRifle*  newWeapon = nullptr;
		newWeapon = GetWorld()->SpawnActor<AMRifle>(newWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, weaponSpawnParams);
		if (newWeapon)
		{
			//currentWeapon->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
			//currentWeapon->DetachRootComponentFromParent();
			//newWeapon->SetActorRelativeLocation(FVector(0, 13, 2));
			newWeapon->SetOwner(this);
			//FAttachmentTransformRules rules(EAttachmentRule::KeepRelative);
			newWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, weaponSocketName);
			newWeapon->SetActorRelativeLocation(FVector(1.34, -11.16, -1.7));
			//allWeapons.Add(newWeapon);
			if (currentWeapon) {
				currentWeapon->stopFire();
				currentWeapon->SetActorHiddenInGame(true);
				currentWeapon->DetachRootComponentFromParent();
				currentWeapon->SetLifeSpan(2);
			}
			GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
			currentWeapon = newWeapon;
		}
	}
}

//void AMShooterCharacter::changeWeapon()
//{
//	return;
//	if (GetLocalRole() != ROLE_Authority) {
//		serverChangeWeapon();
//		//return;
//	}
//
//	//UE_LOG(LogTemp, Warning, TEXT("maohaoran changeWeapon 246"));
//	int size = allWeapons.Num();
//	if (size <= 1)
//		return;
//
//	int curIndex = allWeapons.Find(currentWeapon);
//	if (curIndex == INDEX_NONE)
//		return;
//
//	//UE_LOG(LogTemp, Warning, TEXT("maohaoran changeWeapon 255"));
//	if (currentWeapon) {
//		currentWeapon->SetActorHiddenInGame(true);
//		currentWeapon->DetachRootComponentFromParent();
//	}
//
//	if (curIndex == size - 1)
//		currentWeapon = allWeapons[0];
//	else
//		currentWeapon = allWeapons[++curIndex];
//
//	if (currentWeapon) {
//		//UE_LOG(LogTemp, Warning, TEXT("maohaoran changeWeapon 267"));
//		GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
//		currentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, weaponSocketName);
//		currentWeapon->SetActorRelativeLocation(FVector(1.34, -11.16, -1.7));
//	}
//
//	//currentWeapon->SetActorHiddenInGame(false);
//	
//}
//
//void AMShooterCharacter::serverChangeWeapon_Implementation()
//{
//	changeWeapon();
//}