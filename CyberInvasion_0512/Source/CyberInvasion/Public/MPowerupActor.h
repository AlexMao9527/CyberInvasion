// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MPowerupActor.generated.h"

UCLASS()
class CYBERINVASION_API AMPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMPowerupActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
		void OnActivated(AActor* activeFor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
		void OnExpired();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
		void OnPowerupTicked();

	UFUNCTION()
		void onTickPowerup();

	UFUNCTION()
		void OnRep_PowerupActive();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
		void OnPowerupStateChanged(bool bNewIsPowerupActive);

	void activatePowerup(AActor* activeFor);

	void hideActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	

public:
	UPROPERTY(ReplicatedUsing = OnRep_PowerupActive)
		bool bIsPowerupActive;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
		float powerupIntervals;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
		int32 totalNrOfTicks;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UStaticMeshComponent* meshComp;

	int32 ticksProcessed;

	FTimerHandle poewerupTickTimerHandle;

};
