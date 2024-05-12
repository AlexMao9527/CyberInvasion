// Fill out your copyright notice in the Description page of Project Settings.


#include "MPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "MPowerupActor.h"
#include "Net/UnrealNetwork.h"
#include "MShooterCharacter.h"

// Sets default values
AMPickupActor::AMPickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	sphereComp = CreateDefaultSubobject< USphereComponent>(TEXT("Sphere Comp"));
	RootComponent = sphereComp;
	sphereComp->SetSphereRadius(75);

	decalComp = CreateDefaultSubobject< UDecalComponent>(TEXT("Decal Comp"));
	decalComp->SetupAttachment(RootComponent);
	decalComp->SetRelativeRotation(FRotator(90, 0, 0));
	decalComp->DecalSize = FVector(64, 75, 75);
	cooldownDuration = 5.0f;
	SetReplicates(true);
}

// Called when the game starts or when spawned
void AMPickupActor::BeginPlay()
{
	Super::BeginPlay();
	OnActorBeginOverlap.AddDynamic(this, &AMPickupActor::beginOverlap);

	if (GetLocalRole() == ROLE_Authority)
		respawn();
}

// Called every frame
void AMPickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMPickupActor::beginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	AMShooterCharacter* shooter = Cast< AMShooterCharacter>(OtherActor);
	if (!shooter)
		return;

	if (GetLocalRole() == ROLE_Authority && powerupInstance) {
		//UE_LOG(LogTemp, Warning, TEXT("maohaoran AMPickupActor::beginOverlap ROLE: %d"), GetLocalRole());
		powerupInstance->activatePowerup(OtherActor);
		powerupInstance->hideActor();
		powerupInstance = nullptr;
		GetWorldTimerManager().SetTimer(respawnTimerHandle, this, &AMPickupActor::respawn, cooldownDuration, false, cooldownDuration);
	}
}

void AMPickupActor::respawn()
{
	if (powerupClass == nullptr)
		return;

	FActorSpawnParameters spawnPara;
	spawnPara.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	powerupInstance = GetWorld()->SpawnActor< AMPowerupActor>(powerupClass, GetTransform(), spawnPara); //这里需要调节spawn 的位置
}

void AMPickupActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMPickupActor, powerupInstance);
}