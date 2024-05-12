// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MRifle.generated.h"

USTRUCT()
struct FHitScanTrace {
	GENERATED_BODY()
public:
	UPROPERTY()
		TEnumAsByte<EPhysicalSurface> surfaceType;	//枚举类型若想要网络传输，需声明为TEnumAsByte

	UPROPERTY()
		FVector_NetQuantize traceTo;
};

UCLASS()
class CYBERINVASION_API AMRifle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMRifle();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void fire();
	void startFire();
	void stopFire();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, WithValidation)
		void serverFire();

	UFUNCTION(Server, Reliable, WithValidation)
		void serverStartFire();

	UFUNCTION(Server, Reliable, WithValidation)
		void serverstopFire();

	UFUNCTION()
		void On_RepHitScanTrace();

	void playImpactEffect(EPhysicalSurface surfaceType, FVector impactPoint);
	void playCommonEffect(FVector traceEffectEnd);

private:
	AController* getOwnerController()const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USkeletalMeshComponent* gunMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<class UDamageType> damageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		class UParticleSystem* muzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		class UParticleSystem* fleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		class UParticleSystem* defaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		class UParticleSystem* traceEffect;

	UPROPERTY(EditAnywhere, Category = "Weapon")
		class USoundBase* muzzleSound;
	UPROPERTY(EditAnywhere, Category = "Weapon")
		class USoundBase* impactSoundAI;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName muzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName traceTargetName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		TSubclassOf<UCameraShakeBase> camShakeBaseClass;

	//bullets per minute fired by weapon
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		int rateOfFire;

	UPROPERTY(ReplicatedUsing = On_RepHitScanTrace)
		FHitScanTrace hitScanTrace;

	UPROPERTY(EditAnywhere, Category = "Weapon")
		float baseDamage;

	//子弹随机值（角度）
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0.0f))
		float bulletSpread;


	FTimerHandle timeHandle_timeBetweenShots;

	float lastFireTime;

	//根据rateOfFire计算得来
	float timeBetweenShots;

};
