// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MPickupActor.generated.h"

UCLASS()
class CYBERINVASION_API AMPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMPickupActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void beginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	void respawn();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class USphereComponent* sphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UDecalComponent* decalComp;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
		TSubclassOf<class AMPowerupActor> powerupClass;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
		float cooldownDuration;

	FTimerHandle respawnTimerHandle;

	UPROPERTY(Replicated)
		AMPowerupActor* powerupInstance;
};
