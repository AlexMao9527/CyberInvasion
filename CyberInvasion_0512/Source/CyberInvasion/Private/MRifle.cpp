// Fill out your copyright notice in the Description page of Project Settings.


#include "MRifle.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CyberInvasion/CyberInvasion.h"
#include "Camera/CameraShakeBase.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundBase.h"

// Sets default values
AMRifle::AMRifle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	gunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	RootComponent = gunMesh;
	muzzleSocketName = TEXT("MuzzleFlashSocket");
	traceTargetName = TEXT("BeamEnd");
	rateOfFire = 60;
	baseDamage = 10;
	bulletSpread = 1.0f;
	SetReplicates(true);

	//设置这个是为了减少同步的延迟
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

// Called when the game starts or when spawned
void AMRifle::BeginPlay()
{
	Super::BeginPlay();
	timeBetweenShots = 60.f / (float)rateOfFire;
}

// Called every frame
void AMRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMRifle::fire()
{
	if (GetLocalRole() < ROLE_Authority) {
		serverFire();
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("maohaooran fire localrole:%d"), GetLocalRole());

	AController* ownerController = getOwnerController();
	if (!ownerController)
		return;

	FVector playerLocation;
	FRotator playerRotation;
	ownerController->GetPlayerViewPoint(playerLocation, playerRotation);
	FVector shootDirection = playerRotation.Vector();
	//增加子弹发射随机值
	float halfRad = FMath::DegreesToRadians(bulletSpread);
	shootDirection = FMath::VRandCone(shootDirection, halfRad, halfRad);

	FVector traceEnd = playerLocation + shootDirection * 5000;

	FVector traceEffectEnd = traceEnd;
	FHitResult hitRet;
	FCollisionQueryParams queryPara;
	queryPara.AddIgnoredActor(this);
	queryPara.AddIgnoredActor(GetOwner());
	queryPara.bReturnPhysicalMaterial = true;

	EPhysicalSurface materialType = EPhysicalSurface::SurfaceType_Default;
	bool ret = GetWorld()->LineTraceSingleByChannel(hitRet, playerLocation, traceEnd, WEAPON_CHANNEL, queryPara);
	if (ret && GetOwner() && GetOwner()->GetInstigatorController()) {
		traceEffectEnd = hitRet.ImpactPoint;
		//DrawDebugLine(GetWorld(), playerLocation, traceEnd, FColor::Red, false, 3);

		//Apply Damage
		materialType = UPhysicalMaterial::DetermineSurfaceType(hitRet.PhysMaterial.Get());
		float applyedDamage = baseDamage;
		if (materialType == IMPACT_SURFACE_FLESH_VULNERABLE)
			applyedDamage *= 5.0f;
		UGameplayStatics::ApplyPointDamage(hitRet.GetActor(), applyedDamage, playerRotation.Vector(), hitRet, GetOwner()->GetInstigatorController(), GetOwner(), damageType);
	}

	playCommonEffect(traceEffectEnd);
	playImpactEffect(hitScanTrace.surfaceType, hitScanTrace.traceTo);

	//这里是为了让client也执行特定函数
	if (GetLocalRole() == ROLE_Authority) {
		hitScanTrace.traceTo = traceEffectEnd;
		hitScanTrace.surfaceType = materialType;
	}

	lastFireTime = GetWorld()->TimeSeconds;
}

void AMRifle::startFire()
{
	//为什么要这么做： 因为lastFireTime不是replicated的，在client无法同步，如果将其声明为replicated则还会存在较大延迟。lastFireTime一般是小于1秒的
	if (GetLocalRole() < ROLE_Authority) {
		serverStartFire();
		return;
	}

	float firstDelay = FMath::Max(lastFireTime + timeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	GetWorld()->GetTimerManager().SetTimer(timeHandle_timeBetweenShots, this, &AMRifle::fire, timeBetweenShots, true, firstDelay);
}

void AMRifle::stopFire()
{
	if (GetLocalRole() < ROLE_Authority) {
		serverstopFire();
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("maohaooran stopFire Role: %d"), GetLocalRole());
	GetWorldTimerManager().ClearTimer(timeHandle_timeBetweenShots);
}


AController* AMRifle::getOwnerController()const
{
	APawn* ownerPawn = Cast<APawn>(GetOwner());
	if (!ownerPawn)
		return nullptr;
	AController* ownerController = ownerPawn->GetController();
	return ownerController;
}

void AMRifle::playCommonEffect(FVector traceEffectEnd)
{
	//UE_LOG(LogTemp, Warning, TEXT("maohaooran playCommonEffect Role: %d"),GetLocalRole());
	
	//Apply Muzzle Effect
	if (muzzleEffect)
		UGameplayStatics::SpawnEmitterAttached(muzzleEffect, RootComponent, muzzleSocketName);

	if (muzzleSound)
		UGameplayStatics::SpawnSoundAttached(muzzleSound, gunMesh, muzzleSocketName);

	//Apply trace Effect
	if (traceEffect) {
		FVector beginLoc = gunMesh->GetSocketLocation(muzzleSocketName);
		UParticleSystemComponent* traceEffectComp = UGameplayStatics::SpawnEmitterAtLocation(this, traceEffect, beginLoc);
		if (traceEffectComp)
			traceEffectComp->SetVectorParameter(traceTargetName, traceEffectEnd);
	}

	//player camera shake
	APlayerController* playerCtrller = Cast< APlayerController>(getOwnerController());
	if (playerCtrller) {
		playerCtrller->ClientPlayCameraShake(camShakeBaseClass);
	}

}

void AMRifle::serverFire_Implementation()
{
	fire();
}

bool AMRifle::serverFire_Validate()
{
	return true;
}

void AMRifle::serverStartFire_Implementation()
{
	startFire();
}

bool AMRifle::serverStartFire_Validate()
{
	return true;
}

void AMRifle::serverstopFire_Implementation()
{
	stopFire();
}

bool AMRifle::serverstopFire_Validate()
{
	return true;
}

void AMRifle::On_RepHitScanTrace()
{
	playCommonEffect(hitScanTrace.traceTo);
	playImpactEffect(hitScanTrace.surfaceType, hitScanTrace.traceTo);
}

void AMRifle::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMRifle, hitScanTrace);
}

void AMRifle::playImpactEffect(EPhysicalSurface surfaceType, FVector impactPoint)
{
	//UE_LOG(LogTemp, Warning, TEXT("maohaooran playImpactEffect 173"));
	UParticleSystem* currentPlayEffect = nullptr;

	switch (surfaceType) {
	case IMPACT_SURFACE_FLESH_DEFAULT:
	{
		currentPlayEffect = fleshImpactEffect;
		break;
	}
	case IMPACT_SURFACE_FLESH_VULNERABLE:
	{
		currentPlayEffect = fleshImpactEffect;
		break;
	}
	default:
	{
		currentPlayEffect = defaultImpactEffect;
		break;
	}
	}

	if (impactSoundAI)
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), impactSoundAI, impactPoint);

	if (currentPlayEffect) {
		FVector muzzleLocation = gunMesh->GetSocketLocation(muzzleSocketName);
		FVector shotDirection = impactPoint - muzzleLocation;
		shotDirection.Normalize();
		UGameplayStatics::SpawnEmitterAtLocation(this, currentPlayEffect, impactPoint, shotDirection.Rotation());
	}
}
