// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MGameState.generated.h"


UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WatingToStart,

	WaveInProgress,

	WatingToComplete,

	GameOver,
};

/**
 * 
 */
UCLASS()
class CYBERINVASION_API AMGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void setWaveState(EWaveState newWaveState);

	void setWinnerPlayerName(const FString& playerName);

	UFUNCTION(BlueprintCallable, Category = "GameState")
		FString getWinnerPlayerName() const;

protected:
	UFUNCTION()
		void OnRep_WaveState(EWaveState oldState);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
		void waveStateChanged(EWaveState oldState, EWaveState newState);

protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState, Category = "GameState")
		EWaveState waveState;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "GameState")
		FString winnerPlayerName;
};
