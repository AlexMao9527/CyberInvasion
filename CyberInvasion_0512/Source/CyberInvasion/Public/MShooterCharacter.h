// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MShooterCharacter.generated.h"

extern int bnGameModeType;

UCLASS()
class CYBERINVASION_API AMShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMShooterCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
		void startFire();

	UFUNCTION(BlueprintCallable)
		void stopFire();

	UFUNCTION(NetMulticast, Reliable)
		void netMulticastChangePlayerName();

	UFUNCTION(BlueprintCallable)
		void addNewWeapon(TSubclassOf<class AMRifle> newWeaponClass);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		bool isDead()const;

	UFUNCTION(BlueprintCallable)
		float gethealthPercentage()const;

	UFUNCTION()
		void characterHealthChanged(UMHealthComponent* healthComp, float health, float healthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
		void OnRep_CurMaxWalkSpeed();

	UFUNCTION(BlueprintImplementableEvent)
		void OnSetMaxWalkSpeed(float fNewMaxWaldSpeed);

	UFUNCTION(Server, Reliable)
		void serverChangePlayerName(const FString& playerName);

	UFUNCTION(BlueprintImplementableEvent)
		void onChangePlayerNameWidget();

	UFUNCTION(BlueprintCallable)
		void setPlayerNameWidget(const FString& playerName);

	/*UFUNCTION()
		void changeWeapon();*/

	/*UFUNCTION(Server, Reliable)
		void serverChangeWeapon();*/

	void beginZoom();

	void endZoom();

private:
	void moveForward(float axisValue);
	void moveRight(float axisValue);
	void lookUp(float axisValue);
	void lookRight(float axisValue);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UCameraComponent* cameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USpringArmComponent* springArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UMHealthComponent* healthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
		float zoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 1, ClampMax = 100))
		float zoomInterSpeed;

	UPROPERTY(EditAnywhere, Category = "Weapons")
		TSubclassOf<class AMRifle> weaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
		FName weaponSocketName;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapons")
		bool bIsDead;

	UPROPERTY(Replicated)
		AMRifle* currentWeapon;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_CurMaxWalkSpeed)
		float curMaxWalkSpeed;

private:
	bool wantToZoom;

	float defaultFOV;

	//TArray< AMRifle*> allWeapons;

};
