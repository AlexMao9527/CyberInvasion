// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MTrackerBot.generated.h"

UCLASS()
class CYBERINVASION_API AMTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMTrackerBot();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector getNextPathPoint();

	UFUNCTION()
		void onHealthChanged(UMHealthComponent* healthComp, float health, float healthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
		void beginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	/*UFUNCTION()
		void OnRep_HealthChange();*/

	void selfDestruct();

	FTimerHandle selfDamageTimerHandle;
	FTimerHandle refreshPathTimerHandle;
	//FTimerHandle selfDestructTimerHandle;	

private:
	void damageSelf();
	void refreshPath();

protected:

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* meshComp;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		float movementForce;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		float requiredDistanceToTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UMHealthComponent* healthComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		class USphereComponent* sphereComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TrackerBot")
		class UParticleSystem* explosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		float explosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		float explosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		class USoundCue* selfDestructSound;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
		class USoundCue* explosionSound;

	class UMaterialInstanceDynamic* matInst;

private:
	FVector nextPathToPoint;

	bool bExploded;

	bool bStartedSelfDestructed;
};
