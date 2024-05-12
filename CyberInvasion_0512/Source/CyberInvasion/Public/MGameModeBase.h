// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MGameModeBase.generated.h"

enum class EWaveState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, victimActor, AActor*, killerActor, AController*, killerController);

/**
 * 
 */
UCLASS()
class CYBERINVASION_API AMGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AMGameModeBase();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaTime) override;

	void rebornPlayer(APlayerController* playerController);

	bool gameHasEnded;

protected:

	//蓝图实现生成爆炸球
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
		void spawnNewBot();

	UFUNCTION()
		void rebornPlayerImple(APlayerController* playerController);

	virtual void gameOver();

	void spawnBotTimerElapsed();
	void startWave();
	void endWave();
	void prepareForNextWave();
	void checkWaveState();
	void checkAnyPlayerAlive();
	void setWaveState(EWaveState newWaveState);

public:
	UPROPERTY(BlueprintAssignable, Category = "GameMode")
		FOnActorKilled onActorKilled;

protected:
	FTimerHandle botSpawnerTimerHandle;

	FTimerHandle nextWaveStartTimerhandle;

	//Bots to spawn in current wave
	int NrOfBotsToSpawn;

	int waveCount;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
		float timeBetweenWave;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
		float playerRebornTime;
};
