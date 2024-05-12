// Fill out your copyright notice in the Description page of Project Settings.


#include "MLaunchPad.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "MShooterCharacter.h"

// Sets default values
AMLaunchPad::AMLaunchPad()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	boxComp = CreateDefaultSubobject< UBoxComponent>(TEXT("Box Component"));
	boxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	boxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &AMLaunchPad::OnCollisionDetected);
	RootComponent = boxComp;

	meshCompPlane = CreateDefaultSubobject< UStaticMeshComponent>(TEXT("Mesh Component Plane"));
	meshCompPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	meshCompPlane->SetupAttachment(RootComponent);

	meshCompArrow = CreateDefaultSubobject< UStaticMeshComponent>(TEXT("Mesh Component Arrow"));
	meshCompArrow->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	meshCompArrow->SetupAttachment(RootComponent);

	launchAngle = 60;
	launchMagnitude = 1000;

}

// Called when the game starts or when spawned
void AMLaunchPad::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMLaunchPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMLaunchPad::OnCollisionDetected(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;

	AMShooterCharacter* player = Cast<AMShooterCharacter>(OtherActor);
	if (player) {
		FRotator rotation = GetActorRotation();
		rotation.Pitch += launchAngle;
		FVector launchVelocity = rotation.Vector() * launchMagnitude;

		//add effect
		if (launchFX)
			UGameplayStatics::SpawnEmitterAtLocation(this, launchFX, GetActorLocation());
		if (explosionSound)
			UGameplayStatics::SpawnSoundAttached(explosionSound, RootComponent);
		player->LaunchCharacter(launchVelocity, true, true);
	}
	else {
		//UStaticMeshComponent* rootComp;
		//rootComp = Cast<UStaticMeshComponent>(OtherComp);
		//if (!rootComp)
		//	return;

		////rootComp->AddImpulse(meshImpulse);
		//FRotator rotation = GetActorRotation();
		//rotation.Pitch += launchAngle;
		//FVector launchImpulse = rotation.Vector() * launchMagnitude * 1000;

		////add effect
		//if (launchFX)
		//	UGameplayStatics::SpawnEmitterAtLocation(this, launchFX, GetActorLocation());
		//if (explosionSound)
		//	UGameplayStatics::SpawnSoundAttached(explosionSound, RootComponent);
		//rootComp->AddImpulse(launchImpulse);
	}
}