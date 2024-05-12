// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MGrenade.generated.h"

UCLASS()
class CYBERINVASION_API AMGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMGrenade();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnExplode();

	void playEffect();

	UFUNCTION(NetMulticast, Reliable)
		void netMulticastPlayEffect();

	/*UFUNCTION(Server, Reliable, WithValidation)
		void serverOnExplode();*/

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* GrenadeMeshComp;

	UPROPERTY(EditAnywhere, Category = "Grenade")
		float MaxExplodeTime;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
		class UParticleSystem* explodeEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Explosions")
		class USoundCue* explosionSound;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, Category = "Grenade")
		class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, Category = "Grenade")
		class USphereComponent* sphereComp;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
		float explosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
		float explosionDamage;

private:
	FTimerHandle explodeTimerHandle;
};
