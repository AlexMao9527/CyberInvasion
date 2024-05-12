// Fill out your copyright notice in the Description page of Project Settings.


#include "MShootPlayerController.h"
#include "Engine/EngineTypes.h"
#include "Blueprint/UserWidget.h"

void AMShootPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (GetLocalRole() == ROLE_Authority && HUDWidget) {
		HUDWidgetIns = CreateWidget(this, HUDWidget);
		if (HUDWidgetIns) {
			HUDWidgetIns->AddToViewport();
		}
	}
}

void AMShootPlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);
	if (HUDWidgetIns)
		HUDWidgetIns->RemoveFromViewport();
	if (bIsWinner) {
		if (winWidget)
		{
			UUserWidget* widget = CreateWidget(this, winWidget);
			if (widget)
				widget->AddToViewport();
		}
	}
	else {
		if (loseWidget)
		{
			UUserWidget* widget = CreateWidget(this, loseWidget);
			if (widget)
				widget->AddToViewport();
		}
	}
	GetWorldTimerManager().SetTimer(restartTimer, this, &AMShootPlayerController::RestartLevel, restartDelay);
}