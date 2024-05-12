// Fill out your copyright notice in the Description page of Project Settings.


#include "MGameState.h"
#include "Net/UnrealNetwork.h"


void AMGameState::OnRep_WaveState(EWaveState oldState)
{
	waveStateChanged(oldState, waveState);	//������Ҫע�⣺oldState�����Զ������
}

void AMGameState::setWaveState(EWaveState newWaveState)
{
	if (GetLocalRole() == ROLE_Authority) {
		EWaveState oldWaveState = waveState;
		waveState = newWaveState;
		OnRep_WaveState(oldWaveState);	//�����������Զ����ã����ֶ�����
	}
}

void AMGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMGameState, waveState);
	DOREPLIFETIME(AMGameState, winnerPlayerName);
}

void AMGameState::setWinnerPlayerName(const FString& playerName)
{
	winnerPlayerName = playerName;
}

FString AMGameState::getWinnerPlayerName() const
{
	return winnerPlayerName;
}