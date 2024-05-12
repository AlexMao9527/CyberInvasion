// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MShootPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CYBERINVASION_API AMShootPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void GameHasEnded(class AActor* EndGameFocus = nullptr, bool bIsWinner = false) override;
		
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
		float restartDelay = 5;

	FTimerHandle restartTimer;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> loseWidget;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> winWidget;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> HUDWidget;

	//这里声明为UPROPERTY据说是为了垃圾回收
	UPROPERTY()
		UUserWidget* HUDWidgetIns;
};
