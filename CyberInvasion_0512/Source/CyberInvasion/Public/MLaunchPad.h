// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MLaunchPad.generated.h"

UCLASS()
class CYBERINVASION_API AMLaunchPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMLaunchPad();

	UFUNCTION(BlueprintCallable)
		void OnCollisionDetected(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Component")
		UStaticMeshComponent* meshCompArrow;

	UPROPERTY(VisibleAnywhere, Category = "Component")
		UStaticMeshComponent* meshCompPlane;

	UPROPERTY(VisibleAnywhere, Category = "Component")
		class UBoxComponent* boxComp;

	UPROPERTY(EditInstanceOnly, Category = "Custom Defined")
		int launchAngle;

	UPROPERTY(EditInstanceOnly, Category = "Custom Defined")
		int launchMagnitude;

	UPROPERTY(EditAnywhere, Category = "Custom Defined")
		UParticleSystem* launchFX;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Defined")
		class USoundCue* explosionSound;

};
