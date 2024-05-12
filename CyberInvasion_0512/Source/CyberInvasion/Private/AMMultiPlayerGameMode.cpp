// Fill out your copyright notice in the Description page of Project Settings.


#include "AMMultiPlayerGameMode.h"
#include "CyberInvasion/CyberInvasion.h"
#include "MPlayerState.h"
#include "MMultiPlayerController.h"
#include "MShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "MGameState.h"

AAMMultiPlayerGameMode::AAMMultiPlayerGameMode()
{
	PlayerControllerClass = AMMultiPlayerController::StaticClass();
	DefaultPawnClass = AMShooterCharacter::StaticClass();
	bnGameModeType = GameModeType::Enum_Multi_GameMode;
	winConditionScore = 1000;
}

void AAMMultiPlayerGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	checkHasPlayerWin();
}

void AAMMultiPlayerGameMode::checkHasPlayerWin()
{
	AMMultiPlayerController* winnerCtrl = nullptr;
	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; iter++) {
		AMMultiPlayerController* pc = Cast< AMMultiPlayerController>(iter->Get());
		if (!pc)
			return;
		AMPlayerState* playerState = Cast< AMPlayerState>(pc->PlayerState);
		if (!playerState)
			return;

		if (playerState->Score >= winConditionScore) {
			winnerCtrl = pc;
			AMGameState* mGameState = Cast< AMGameState>(GetWorld()->GetGameState());
			if (mGameState)
				mGameState->setWinnerPlayerName(playerState->GetPlayerName());

		}
	}

	if (!winnerCtrl)	//没有玩家胜出
		return;

	gameOver();
	for (FConstPlayerControllerIterator iter = GetWorld()->GetPlayerControllerIterator(); iter; iter++) {
		AMMultiPlayerController* pc = Cast< AMMultiPlayerController>(iter->Get());
		if (!pc)
			continue;
		if (pc == winnerCtrl)
			pc->GameHasEnded(nullptr, true);
		else
			pc->GameHasEnded(nullptr, false);
	}

}

void AAMMultiPlayerGameMode::refreshAllPlayerName()
{
	TArray<AActor*> allPlayerPawns;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMShooterCharacter::StaticClass(), allPlayerPawns);
	for (AActor* playerPawn : allPlayerPawns)
	{
		if (!playerPawn)
			continue;

		AMShooterCharacter* pCharacter = Cast< AMShooterCharacter>(playerPawn);
		if (!pCharacter)
			continue;

		pCharacter->netMulticastChangePlayerName();
	}
}