// Fill out your copyright notice in the Description page of Project Settings.


#include "MSinglePlayerGameMode.h"
#include "MGameState.h"
#include "EngineUtils.h"
#include "MShootPlayerController.h"
#include "Components/MHealthComponent.h"
#include "CyberInvasion/CyberInvasion.h"

AMSinglePlayerGameMode::AMSinglePlayerGameMode()
{
	bnGameModeType = GameModeType::Enum_Single_GameMode;
	numberOfWaveCount = 5;
}

void AMSinglePlayerGameMode::Tick(float DeltaTime)
{
	if(!gameHasEnded)
		this->checkWaveState();
}

void AMSinglePlayerGameMode::checkWaveState()
{
	bool isPreparingForNextWave = GetWorldTimerManager().IsTimerActive(nextWaveStartTimerhandle);
	if (NrOfBotsToSpawn > 0 || isPreparingForNextWave)
		return;

	bool isAnyBotAlive = false;

	for (FConstPawnIterator iter = GetWorld()->GetPawnIterator(); iter; iter++) {
		APawn* curPawn = iter->Get();
		if (!curPawn)
			continue;

		APlayerController* playerController = Cast< APlayerController>(curPawn->GetController());
		if (playerController)
			continue;

		UMHealthComponent* healComp = Cast< UMHealthComponent>(curPawn->GetComponentByClass(UMHealthComponent::StaticClass()));
		if (healComp && healComp->getHealth() > 0.0f) {
			isAnyBotAlive = true;
			break;
		}
	}

	if (!isAnyBotAlive && waveCount < numberOfWaveCount)
		prepareForNextWave();
	else if (!isAnyBotAlive && waveCount >= numberOfWaveCount) {
		gameHasEnded = true;
		gameOverCustom(true);
	}
}

void AMSinglePlayerGameMode::gameOverCustom(bool isWin)
{
	endWave();
	setWaveState(EWaveState::GameOver);

	TActorRange allControllerArray = TActorRange<AMShootPlayerController>(GetWorld());
	for (AMShootPlayerController* controller : allControllerArray) {
		if (!controller)
			continue;
		controller->GameHasEnded(nullptr, isWin);
	}
}
