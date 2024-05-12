// Fill out your copyright notice in the Description page of Project Settings.


#include "MTrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Particles/ParticleSystem.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"
#include "Net/UnrealNetwork.h"
#include "MShooterCharacter.h"
#include "Components/MHealthComponent.h"

#define MAX_DISTANCE_TO_PAWN 1000000

// Sets default values
AMTrackerBot::AMTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	meshComp->SetCanEverAffectNavigation(false);
	RootComponent = meshComp;
	healthComponent = CreateDefaultSubobject< UMHealthComponent>(TEXT("Health Comp"));
	explosionRadius = 200;
	sphereComp = CreateDefaultSubobject< USphereComponent>(TEXT("Sphere Comp"));
	sphereComp->SetupAttachment(RootComponent);
	sphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	sphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	sphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	sphereComp->SetSphereRadius(explosionRadius);


	meshComp->SetSimulatePhysics(true);
	bUseVelocityChange = false;
	movementForce = 1500;
	requiredDistanceToTarget = 100;
	bExploded = false;
	explosionDamage = 50;
	explosionRadius = 200;
	bStartedSelfDestructed = false;
	//currentHealth = healthComponent->health;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AMTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	healthComponent->onHealthChanged.AddDynamic(this, &AMTrackerBot::onHealthChanged);
	OnActorBeginOverlap.AddDynamic(this, &AMTrackerBot::beginOverlap);
	GetWorldTimerManager().SetTimer(refreshPathTimerHandle, this, &AMTrackerBot::refreshPath, 5.0f, true, 0.0f);
	SetLifeSpan(120);	//设置生存周期为2分钟，2分钟后自动销毁 (不能写到构造函数中，会出错)
}

// Called every frame
void AMTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_Authority && !bExploded) {
		float distanceToTarget = (GetActorLocation() - nextPathToPoint).Size();
		if (distanceToTarget <= requiredDistanceToTarget) {
			nextPathToPoint = getNextPathPoint();
		}
		else {
			FVector forceDirection = nextPathToPoint - GetActorLocation();
			forceDirection.Normalize();
			forceDirection *= movementForce;

			meshComp->AddForce(forceDirection, NAME_None, bUseVelocityChange);
			//DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + forceDirection, 32, FColor::Yellow, 0, 0, 1);

		}
		//DrawDebugSphere(GetWorld(), GetActorLocation(), 50, 20, FColor::Green, false, 0, 1);
	}
}

// Called to bind functionality to input
void AMTrackerBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FVector AMTrackerBot::getNextPathPoint()
{
	//step1:找到最近的玩家
	float minDistanceToPawn = MAX_DISTANCE_TO_PAWN;
	APawn* nearestPlayerPawn = nullptr;

	for (FConstPawnIterator iter = GetWorld()->GetPawnIterator(); iter; iter++) {
		APawn* pawn = iter->Get();
		if (!pawn || UMHealthComponent::isFriendly(this, pawn))
			continue;

		UMHealthComponent* pawnHealComp = Cast< UMHealthComponent>(pawn->GetComponentByClass(UMHealthComponent::StaticClass()));

		if (pawnHealComp && pawnHealComp->getHealth() > 0.0f) {
			float distance = (GetActorLocation() - pawn->GetActorLocation()).Size();
			if (distance < minDistanceToPawn) {
				minDistanceToPawn = distance;
				nearestPlayerPawn = pawn;
			}
		}
	}

	if (!nearestPlayerPawn)
		return GetActorLocation();

	//step2:返回路径
	UNavigationPath* navPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), nearestPlayerPawn);
	if (navPath && navPath->PathPoints.Num() > 1) {
		return navPath->PathPoints[1];
	}

	return GetActorLocation();
}

void AMTrackerBot::onHealthChanged(UMHealthComponent* healthComp, float health, float healthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	//在游戏运行时动态改变材质参数，实现材质的动态效果
	//UE_LOG(LogTemp, Warning, TEXT("maohaoran 61: %d"), GetLocalRole());

	//currentHealth = health;
	if (matInst == nullptr) {
		matInst = meshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, meshComp->GetMaterial(0));
	}

	if (matInst) {
		matInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	if (health <= 0.0f) {
		selfDestruct();
	}
}

void AMTrackerBot::selfDestruct()
{
	if (bExploded)
		return;

	bExploded = true;

	if (explosionEffect) {
		UGameplayStatics::SpawnEmitterAtLocation(this, explosionEffect, GetActorLocation());
	}

	if (explosionSound) {
		UGameplayStatics::SpawnSoundAtLocation(this, explosionSound, GetActorLocation());
	}

	meshComp->SetVisibility(false);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (GetLocalRole() == ROLE_Authority) {
		TArray<AActor*> ignoreActors;
		ignoreActors.Add(this);

		UGameplayStatics::ApplyRadialDamage(this, explosionDamage, GetActorLocation(), explosionRadius, nullptr, ignoreActors, this, GetInstigatorController(), true);

		//DrawDebugSphere(GetWorld(), GetActorLocation(), explosionRadius, 20, FColor::Red, false, 3);

		//Destroy();
		SetLifeSpan(2.0f);
	}
}

void AMTrackerBot::beginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (bStartedSelfDestructed || bExploded)
		return;

	if (UMHealthComponent::isFriendly(this, OtherActor)) {
		return;
	}

	AMShooterCharacter* playerPawn = Cast< AMShooterCharacter>(OtherActor);
	if (playerPawn) {
		if (GetLocalRole() == ROLE_Authority)
			GetWorldTimerManager().SetTimer(selfDamageTimerHandle, this, &AMTrackerBot::damageSelf, 0.5f, true, 0.0f);

		bStartedSelfDestructed = true;
		UGameplayStatics::SpawnSoundAttached(selfDestructSound, RootComponent);
	}
}

void AMTrackerBot::damageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

void AMTrackerBot::refreshPath()
{
	nextPathToPoint = getNextPathPoint();
}