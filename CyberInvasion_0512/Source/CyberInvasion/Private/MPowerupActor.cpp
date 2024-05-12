// Fill out your copyright notice in the Description page of Project Settings.


#include "MPowerupActor.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMPowerupActor::AMPowerupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	powerupIntervals = 0.0f;
	totalNrOfTicks = 0;
	ticksProcessed = 0;
	bIsPowerupActive = false;

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Comp"));
	RootComponent = meshComp;
	SetReplicates(true);
}

// Called when the game starts or when spawned
void AMPowerupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMPowerupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMPowerupActor::onTickPowerup()
{
	ticksProcessed++;
	OnPowerupTicked();

	if (ticksProcessed >= totalNrOfTicks) {
		OnExpired();
		bIsPowerupActive = false;
		GetWorldTimerManager().ClearTimer(poewerupTickTimerHandle);
		Destroy();
	}
}

void AMPowerupActor::activatePowerup(AActor* activeFor)
{
	OnActivated(activeFor);
	OnPowerupStateChanged(bIsPowerupActive);
	bIsPowerupActive = true;

	if (powerupIntervals > 0.0f) {
		GetWorldTimerManager().SetTimer(poewerupTickTimerHandle, this, &AMPowerupActor::onTickPowerup, powerupIntervals, true, powerupIntervals);
	}
	else {
		onTickPowerup();
	}
}

void AMPowerupActor::hideActor()
{
	meshComp->SetVisibility(false);
}

void AMPowerupActor::OnRep_PowerupActive()
{
	OnPowerupStateChanged(bIsPowerupActive);
}

void AMPowerupActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMPowerupActor, bIsPowerupActive);
}