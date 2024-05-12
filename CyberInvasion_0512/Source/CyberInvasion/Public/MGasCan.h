// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MGasCan.generated.h"

UCLASS()
class CYBERINVASION_API AMGasCan : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMGasCan();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void gasCanOnHealthChanged(UMHealthComponent* healthComp, float health, float healthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
		void On_RepDead();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UStaticMeshComponent* gasCanMeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Explosions")
		class UMaterialInterface* gasCanMIExploded;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UMHealthComponent* healthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Explosions")
		FVector explosedImpulseToSelf;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosions")
		class UParticleSystem* explodeEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class URadialForceComponent* radicalForceComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosions")
		float damage;

	UPROPERTY(ReplicatedUsing = On_RepDead)
		bool isDead;

	UPROPERTY(EditDefaultsOnly, Category = "Explosions")
		class USoundCue* explosionSound;

private:
	class UMaterialInstanceDynamic* gasCanMIDExploded;

};
