// Fill out your copyright notice in the Description page of Project Settings.


#include "MPlayerState.h"
#include "Net/UnrealNetwork.h"

AMPlayerState::AMPlayerState()
{
	Score = 0;
	SetReplicates(true);
}

void AMPlayerState::addScore(float scoreDelta)
{
	if (GetLocalRole() < ROLE_Authority) {
		serverAddScore(scoreDelta);
		return;
	}
	
	Score += scoreDelta;
}

float AMPlayerState::getScore() const
{
	return Score;
}

void AMPlayerState::serverAddScore_Implementation(float scoreDelta)
{
	addScore(scoreDelta);
}