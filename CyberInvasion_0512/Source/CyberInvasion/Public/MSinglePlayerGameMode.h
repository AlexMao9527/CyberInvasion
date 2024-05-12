// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MGameModeBase.h"
#include "MSinglePlayerGameMode.generated.h"

extern int bnGameModeType;

/**
 * 
 */
UCLASS()
class CYBERINVASION_API AMSinglePlayerGameMode : public AMGameModeBase
{
	GENERATED_BODY()
public:

	AMSinglePlayerGameMode();

	UFUNCTION(BlueprintCallable, Category = "GameMode")
		void gameOverCustom(bool isWin);

	virtual void Tick(float DeltaTime) override;

protected:
	void checkWaveState();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
		int numberOfWaveCount;	//总共产生的AI潮数量
};
