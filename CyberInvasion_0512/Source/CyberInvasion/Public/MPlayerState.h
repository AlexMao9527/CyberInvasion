// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class CYBERINVASION_API AMPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	AMPlayerState();

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
		void addScore(float scoreDelta);

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
		float getScore() const;

	UFUNCTION(Server, Reliable)
		void serverAddScore(float scoreDelta);
};
