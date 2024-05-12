// Fill out your copyright notice in the Description page of Project Settings.


#include "MGrenade.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "MShooterCharacter.h"
#include "Sound/SoundCue.h"

// Sets default values
AMGrenade::AMGrenade()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GrenadeMeshComp = CreateDefaultSubobject< UStaticMeshComponent>(TEXT("Grenade Static Mesh"));
	GrenadeMeshComp->SetSimulatePhysics(true);
	RootComponent = GrenadeMeshComp;
	//GrenadeMeshComp->SetupAttachment(RootComponent);

	explosionRadius = 500;
	explosionDamage = 200;

	sphereComp = CreateDefaultSubobject< USphereComponent>(TEXT("Sphere Comp"));
	sphereComp->SetupAttachment(RootComponent);
	sphereComp->SetSphereRadius(explosionRadius);
	sphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	//sphereComp->OnComponentBeginOverlap.AddDynamic(this, &AMGrenade::OnOverlapBegin);

	MaxExplodeTime = 2;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = RootComponent;
	/*ProjectileMovement->InitialSpeed = 2500.f;
	ProjectileMovement->MaxSpeed = 2500.f;*/
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 2000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	SetReplicates(true);
}

// Called when the game starts or when spawned
void AMGrenade::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(explodeTimerHandle, this, &AMGrenade::OnExplode, MaxExplodeTime, false);
}

// Called every frame
void AMGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMGrenade::OnExplode()
{
	//UE_LOG(LogTemp, Warning, TEXT("maohaoran OnExplode localrole:%d"), GetLocalRole());

	playEffect();
	if (GetLocalRole() == ROLE_Authority)
		netMulticastPlayEffect();

	APawn* ownerPawn = Cast<APawn>(GetOwner());
	if (!ownerPawn)
		return;

	//apply damage
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	//UGameplayStatics::ApplyRadialDamageWithFalloff(this, explosionDamage, explosionDamage / 2, GetActorLocation(), explosionRadius * 0.8, explosionRadius, 2, UDamageType::StaticClass(), IgnoreActors, GetOwner(), ownerPawn->GetController());
	//UGameplayStatics::ApplyRadialDamage(this, explosionDamage, GetActorLocation()+FVector(0,0,30), explosionRadius, nullptr, IgnoreActors, GetOwner());

	//因为传统伤害制造不起效果，所以采用如下麻烦的方法
	TArray<AActor*> overlappingActors;
	sphereComp->GetOverlappingActors(overlappingActors,AActor::StaticClass());
	for (int i = 0; i < overlappingActors.Num(); i++) {
		AActor* curActor = overlappingActors[i];
		if (!curActor)
			continue;
		
		//AController* ctrl = Cast<GetOwner()
		AMShooterCharacter* owner = Cast< AMShooterCharacter>(GetOwner());
		if (owner && owner->GetController() && curActor!= owner) {
			//UE_LOG(LogTemp, Warning, TEXT("maohaoran Overlap with %s"), *curActor->GetName());
			UGameplayStatics::ApplyDamage(curActor, explosionDamage, owner->GetController(), GetOwner(), nullptr);
		}
	}

	GetWorldTimerManager().ClearTimer(explodeTimerHandle);
	Destroy();
}

void AMGrenade::playEffect()
{
	if (explodeEffect) {
		UGameplayStatics::SpawnEmitterAtLocation(this, explodeEffect, this->GetActorLocation(), this->GetActorRotation());
	}
	if (explosionSound)
		UGameplayStatics::SpawnSoundAttached(explosionSound, RootComponent);
}

void AMGrenade::netMulticastPlayEffect_Implementation()
{
	playEffect();
}

void AMGrenade::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	return;
	// 在这里处理重叠的 Actor
	if (OtherActor && OtherActor != this)
	{
		// 在这里处理你需要的逻辑，例如打印重叠的 Actor 的名称
		//UE_LOG(LogTemp, Warning, TEXT("maohaoran Overlap with %s"), *OtherActor->GetName());
	}
}