// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MRifle.h"
#include "MGrenadeLauncher.generated.h"

/**
 * 
 */
UCLASS()
class CYBERINVASION_API AMGrenadeLauncher : public AMRifle
{
	GENERATED_BODY()

public:
	AMGrenadeLauncher();

	virtual void fire();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		TSubclassOf<class AMGrenade> grenadeClass;
};
