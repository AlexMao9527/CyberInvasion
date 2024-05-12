// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MMultiPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class CYBERINVASION_API AMMultiPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	enum WidgetType {
		Enum_WidgetType_HUD = 0,
		Enum_WidgetType_Lose,
		Enum_WidgetType_Win,
		Enum_WidgetType_Init,
	};

	AMMultiPlayerController();

	virtual void GameHasEnded(class AActor* EndGameFocus = nullptr, bool bIsWinner = false) override;

	UFUNCTION(BlueprintCallable)
		void changeWidget(int widgetType);

	UPROPERTY(Replicated)
		uint8 playerTeam;	//玩家所属队伍

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Client, Reliable, WithValidation)
		void clientChangeWidget(int widgetType);

private:
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> HUDWidget;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> loseWidget;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> winWidget;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> initWidget;

	//这里声明为UPROPERTY据说是为了垃圾回收
	UPROPERTY()
		UUserWidget* HUDWidgetIns;
};
