// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UMHealthComponent*, healthComp, float, health, float, healthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

extern int bnGameModeType;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CYBERINVASION_API UMHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMHealthComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float getHealth();

	float getHealthPercentage();

	UFUNCTION(BlueprintCallable)
		void heal(float healAmount);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HealthComponents")
		static bool isFriendly(AActor* actorA, AActor* actorB);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
		void handleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
		void OnRep_Health(float oldHealth);

public:

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnHealthChangedSignature onHealthChanged;

	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category = "HealthComponents")
		uint8 teamNum;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Health, EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponents")
		float health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponents")
		float defaultHealth;

private:
	bool bIsDead;

		
};
