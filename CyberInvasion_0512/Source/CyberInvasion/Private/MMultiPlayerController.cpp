// Fill out your copyright notice in the Description page of Project Settings.


#include "MMultiPlayerController.h"
#include "Engine/EngineTypes.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"

AMMultiPlayerController::AMMultiPlayerController()
{
	SetReplicates(true);
}

void AMMultiPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (GetLocalRole() < ROLE_Authority) {
		changeWidget(Enum_WidgetType_Init);
	}
}

void AMMultiPlayerController::changeWidget(int widgetType)
{
	if (GetLocalRole() == ROLE_Authority) {
		clientChangeWidget(widgetType);
		return;
	}

	if (HUDWidgetIns)
		HUDWidgetIns->RemoveFromViewport();
	switch (widgetType) {
	case Enum_WidgetType_HUD:
		if (HUDWidget) {
			HUDWidgetIns = CreateWidget(this, HUDWidget);
		}
		break;
	case Enum_WidgetType_Lose:
		if (loseWidget) {
			HUDWidgetIns = CreateWidget(this, loseWidget);
		}
		break;
	case Enum_WidgetType_Win:
		if (winWidget) {
			HUDWidgetIns = CreateWidget(this, winWidget);
		}
		break;
	case Enum_WidgetType_Init:
		if (initWidget) {
			HUDWidgetIns = CreateWidget(this, initWidget);
		}
		break;
	default:
		if (HUDWidget) {
			HUDWidgetIns = CreateWidget(this, HUDWidget);
		}
		break;
	}
	if (HUDWidgetIns) {
		HUDWidgetIns->AddToViewport();
	}
}

void AMMultiPlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);

	UnPossess();
	
	if (bIsWinner) {
		changeWidget(Enum_WidgetType_Win);
	}
	else {
		changeWidget(Enum_WidgetType_Lose);
	}
}

void AMMultiPlayerController::clientChangeWidget_Implementation(int widgetType)
{
	changeWidget(widgetType);
}

bool AMMultiPlayerController::clientChangeWidget_Validate(int widgetType)
{
	return true;
}

void AMMultiPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMMultiPlayerController, playerTeam);
}