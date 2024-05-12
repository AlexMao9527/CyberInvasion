// Fill out your copyright notice in the Description page of Project Settings.


#include "MGrenadeLauncher.h"
#include "MGrenade.h"

AMGrenadeLauncher::AMGrenadeLauncher()
{
	muzzleSocketName = TEXT("MuzzleFlashSocket");
}

void AMGrenadeLauncher::BeginPlay()
{
	Super::BeginPlay();
}

void AMGrenadeLauncher::fire()
{
	if (GetLocalRole() < ROLE_Authority) {
		serverFire();
		return;
	}

	APawn* owner = Cast<APawn>(GetOwner());
	if (!owner)
		return;
	//(X = 0.101661, Y = 9.934114, Z = -3.334596)
	if (grenadeClass)
	{
		FVector MuzzleLocation = gunMesh->GetSocketLocation(muzzleSocketName);
		FRotator MuzzleRotation = gunMesh->GetSocketRotation(muzzleSocketName);
		FVector playerLocation;
		FRotator playerRotation;

		if (!owner->GetController())
			return;
		owner->GetController()->GetPlayerViewPoint(playerLocation, playerRotation);

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ActorSpawnParams.Instigator = owner;

		playCommonEffect(FVector());
		//playImpactEffect(hitScanTrace.surfaceType, hitScanTrace.traceTo);

		//����Ϊ����ReplicatedUsing �ܹ��������Ӷ�ʹ����clientҲ�ܹ�����playCommonEffect ������ʵ������Щ����û���κ����壩
		if (GetLocalRole() == ROLE_Authority) {
			hitScanTrace.traceTo = hitScanTrace.traceTo + 1;
		}

		// spawn the projectile at the muzzle
		AMGrenade* grenade = GetWorld()->SpawnActor<AMGrenade>(grenadeClass, MuzzleLocation, playerRotation, ActorSpawnParams);
		if (grenade)
			grenade->SetOwner(owner);
	}
	lastFireTime = GetWorld()->TimeSeconds;
}