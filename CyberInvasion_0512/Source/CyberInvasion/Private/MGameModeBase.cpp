// Fill out your copyright notice in the Description page of Project Settings.


#include "MGameModeBase.h"
#include "MGameState.h"
#include "MPlayerState.h"
#include "Components/MHealthComponent.h"


AMGameModeBase::AMGameModeBase()
{
	timeBetweenWave = 3.0f;
	playerRebornTime = 5.0f;
	waveCount = 0;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
	GameStateClass = AMGameState::StaticClass();
	PlayerStateClass = AMPlayerState::StaticClass();
	gameHasEnded = false;
}

void AMGameModeBase::Tick(float DeltaTime)
{
	if(!gameHasEnded)
		checkWaveState();
}

void AMGameModeBase::gameOver()
{
	endWave();
	setWaveState(EWaveState::GameOver);
}

void AMGameModeBase::StartPlay()
{
	Super::StartPlay();
	prepareForNextWave();
}

void AMGameModeBase::startWave()
{
	waveCount++;
	NrOfBotsToSpawn = 2 * waveCount;
	GetWorldTimerManager().SetTimer(botSpawnerTimerHandle, this, &AMGameModeBase::spawnBotTimerElapsed, 1.0f, true, 0.0f);
	setWaveState(EWaveState::WaveInProgress);
}

void AMGameModeBase::endWave()
{
	GetWorldTimerManager().ClearTimer(botSpawnerTimerHandle);
	setWaveState(EWaveState::WatingToComplete);
}

void AMGameModeBase::prepareForNextWave()
{
	//nextWaveStartTimerhandle;
	GetWorldTimerManager().SetTimer(nextWaveStartTimerhandle, this, &AMGameModeBase::startWave, timeBetweenWave, false, timeBetweenWave);
	setWaveState(EWaveState::WatingToStart);
}

void AMGameModeBase::spawnBotTimerElapsed()
{
	spawnNewBot();
	NrOfBotsToSpawn--;
	if (NrOfBotsToSpawn <= 0) {
		endWave();
	}
}

void AMGameModeBase::checkWaveState()
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

	if (!isAnyBotAlive)
		prepareForNextWave();
}

void AMGameModeBase::checkAnyPlayerAlive()
{
	//GetWorld()->GetPlayerControllerIterator
	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; iter++) {
		APlayerController* pc = iter->Get();
		if (pc && pc->GetPawn()) {
			APawn* myPawn = pc->GetPawn();
			UMHealthComponent* healComp = Cast< UMHealthComponent>(myPawn->GetComponentByClass(UMHealthComponent::StaticClass()));
			if (ensure(healComp) && healComp->getHealth() > 0.0f) {
				return;
			}
		}
	}
}

void AMGameModeBase::setWaveState(EWaveState newWaveState)
{
	AMGameState* gs = GetGameState< AMGameState>();
	if (ensureAlways(gs)) {
		gs->setWaveState(newWaveState);
	}
}

void AMGameModeBase::rebornPlayer(APlayerController* playerController)
{
	if (!playerController)
		return;

	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, FName("rebornPlayerImple"), playerController);

	FTimerHandle playerRebornTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(playerRebornTimerHandle, TimerDel, playerRebornTime, false);
	//GetWorldTimerManager().SetTimer(playerRebornTimerHandle, this, &AMGameModeBase::spawnBotTimerElapsed, 1.0f, true, 0.0f);
}

void AMGameModeBase::rebornPlayerImple(APlayerController* playerController)
{
	//RestartPlayer(playerController);
	if (playerController == nullptr || playerController->IsPendingKillPending())
	{
		return;
	}

	AActor* StartSpot = ChoosePlayerStart(playerController);

	// If a start spot wasn't found,
	if (StartSpot == nullptr)
	{
		// Check for a previously assigned spot
		if (playerController->StartSpot != nullptr)
		{
			StartSpot = playerController->StartSpot.Get();
		}
	}

	RestartPlayerAtPlayerStart(playerController, StartSpot);
}
