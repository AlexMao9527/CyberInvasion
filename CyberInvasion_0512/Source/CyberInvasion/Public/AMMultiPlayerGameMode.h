// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MGameModeBase.h"
#include "AMMultiPlayerGameMode.generated.h"

extern int bnGameModeType;

/**
 * 
 */
UCLASS()
class CYBERINVASION_API AAMMultiPlayerGameMode : public AMGameModeBase
{
	GENERATED_BODY()
	
public:

	AAMMultiPlayerGameMode();

	virtual void Tick(float DeltaTime) override;

	void refreshAllPlayerName();

private:
	void checkHasPlayerWin();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
		int winConditionScore;	//玩家胜利条件，分数

};
