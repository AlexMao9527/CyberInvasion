// Fill out your copyright notice in the Description page of Project Settings.


#include "MGasCan.h"
#include "Components/MHealthComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"

// Sets default values
AMGasCan::AMGasCan()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	gasCanMeshComp = CreateDefaultSubobject< UStaticMeshComponent>(TEXT("Gas Can Mesh Comp"));
	gasCanMeshComp->SetSimulatePhysics(true);
	RootComponent = gasCanMeshComp;

	healthComponent = CreateDefaultSubobject< UMHealthComponent>(TEXT("Health Comp"));
	healthComponent->onHealthChanged.AddDynamic(this, &AMGasCan::gasCanOnHealthChanged);

	isDead = false;
	damage = 100;
	explosedImpulseToSelf = FVector(0, 0, 5000);

	radicalForceComp = CreateDefaultSubobject< URadialForceComponent>(TEXT("Radical Force Component"));
	radicalForceComp->SetupAttachment(RootComponent);
	radicalForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);
	radicalForceComp->Radius = 500;
	radicalForceComp->ImpulseStrength = 500;
	radicalForceComp->bImpulseVelChange = true;  //忽略质量影响，可以排除一些质量不一样的问题
	radicalForceComp->bAutoActivate = true;
	radicalForceComp->bIgnoreOwningActor = true;
	SetReplicates(true);
	SetReplicateMovement(true);

}

// Called when the game starts or when spawned
void AMGasCan::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMGasCan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMGasCan::gasCanOnHealthChanged(UMHealthComponent* healthComp, float health, float healthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (health <= 0.0f && !isDead) {
		isDead = true;

		On_RepDead();
		UGameplayStatics::ApplyRadialDamage(this, damage, GetActorLocation(), radicalForceComp->Radius, nullptr, TArray<AActor*>());
		radicalForceComp->FireImpulse();
	}
}

void AMGasCan::On_RepDead()
{
	gasCanMeshComp->AddImpulse(explosedImpulseToSelf);
	if (gasCanMIExploded) {
		gasCanMIDExploded = UMaterialInstanceDynamic::Create(gasCanMIExploded, gasCanMeshComp);
		gasCanMeshComp->SetMaterial(0, gasCanMIDExploded);
	}

	if (explodeEffect) {
		UGameplayStatics::SpawnEmitterAtLocation(this, explodeEffect, this->GetActorLocation(), this->GetActorRotation());
	}

	if (explosionSound)
		UGameplayStatics::SpawnSoundAttached(explosionSound,RootComponent);

	//radicalForceComp->FireImpulse();
}

void AMGasCan::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMGasCan, isDead);
}

